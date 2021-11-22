
#include <visualization/OctreeVT.h>

#include <RenderBase/defines.h>

#include <glm/glm.hpp>

#include <glm/gtx/string_cast.hpp>

#define BRANCHING_FACTOR 2

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
#define SHADER_MACROS gl::ShaderDefines{"DEBUG", "BRANCHING_FACTOR " STRINGIFY(BRANCHING_FACTOR)}

using namespace std;
using namespace rb;

OctreeVT::OctreeVT() :
    program(
        make_shared<gl::Shader>(GL_VERTEX_SHADER,   RESOURCE_SHADERS_NODE_VS),
        make_shared<gl::Shader>(GL_GEOMETRY_SHADER, RESOURCE_SHADERS_NODE_GS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_NODE_FS)
    ),
    octreeEvaluationProgram(
        make_shared<gl::Shader>(GL_COMPUTE_SHADER, RESOURCE_SHADERS_VSO_EVALUATE_BRICK_COMP, SHADER_MACROS)
    ),
    octreeInitiationProgram(
        make_shared<gl::Shader>(GL_COMPUTE_SHADER, RESOURCE_SHADERS_VSO_INIT_NEW_LEVEL_NODES_COMP, SHADER_MACROS)
    )
{
    // Octree construction testing
    // ---------------------------
    // NOTE: this should be done only once
    
    // initial constant settings -> TODO: set this via gui
    const uint32 maxSubdivisions   = 3;
    const uint32 branchingFactor   = BRANCHING_FACTOR;
    const uint32 preallocatedNodes = 1000;
    const uint32 nodesPerTile      = branchingFactor * branchingFactor * branchingFactor;
    
    
    // octree dynamic datastructure on GPU
    nodeCount                     = nodesPerTile; // one single initial tile
    uint32 currentLevel           = 0;
    uint32 currentLevelBeginIndex = 0;
    uint32 nodesInCurrentLevel    = nodeCount;
    counterBuffer = make_unique<gl::Buffer>(sizeof(uint32), &nodeCount, GL_DYNAMIC_DRAW);
    nodeBuffer    = make_unique<gl::Buffer>(sizeof(uint32) * preallocatedNodes, nullptr, GL_DYNAMIC_DRAW);
    vertexBuffer  = make_unique<gl::Buffer>(sizeof(glm::vec4) * preallocatedNodes, nullptr, GL_DYNAMIC_DRAW);
    
    // bind those buffers
    counterBuffer->bindBase(GL_ATOMIC_COUNTER_BUFFER, 0);
    nodeBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
    
    // render content of those buffers for debug reasons
    auto printLevel = [&](const char* caption) {
        auto nodes = vector<uint32>(nodeCount, 0);
        auto verticies = vector<glm::vec4>(nodeCount, glm::vec4(0));
        
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        nodeBuffer->getData(nodes);
        vertexBuffer->getData(verticies);
        
        RB_DEBUG(caption << ":");
        RB_DEBUG("  beginNodeIndex: " << currentLevelBeginIndex);
        RB_DEBUG("  nodesInLevel: " << nodesInCurrentLevel);
        RB_DEBUG("  currentLevel: " << currentLevel);
        RB_DEBUG("  Tiles:");
        for (int tileIndex = 0; tileIndex < nodeCount / nodesPerTile; ++tileIndex) {
            RB_DEBUG("    Tile: " << tileIndex);
            for (int localIndex = 0; localIndex < nodesPerTile; ++localIndex) {
                int i = tileIndex * nodesPerTile + localIndex;
                RB_DEBUG("      (" << i << ") " << localIndex << ":  " <<
                    ((nodes[i] & 0x80000000) ? "1" : "0") <<
                    " | " <<((nodes[i] & 0x40000000) ? "1" : "0") <<
                    " | " << (nodes[i] & 0x3FFFFFFF) <<
                    "   " << glm::to_string(verticies[i])
                );
            }
        }
        RB_DEBUG(" ");
    };
    
    // program - global setup
    
    // definitions of program algorithm stages:
    
    // STAGE 1 generating volume and subdividing on demand
    // Current SVO level is initialized and each tile is zeroed and has defined its position.
    // Spawn a workgroup for each node tile which will compute a brick volume associated with it.
    // Each node tile is evaluated using 8x8x8 thrreads one for each voxel in the volume.
    // This threads will calculate and store distances an base on that they will vote to subdivide the node or not to.
    // If at least one voxel intersects with the volume surface, the nod will get divided.
    // The division will be doe by first thread in workgroup (if any thread requests division) by incrementing node counter
    // to hold new node tile. The tile will be uninitialized so next stage will need to be computed.
    
    auto evaluateLevel = [&]() {
        octreeEvaluationProgram.use();
        octreeEvaluationProgram.uniform("levelBeginIndex", currentLevelBeginIndex);
        
        // one brick evaluated in one workgroup per tile
        RB_DEBUG("Evaluating " << nodesInCurrentLevel << " bricks at level " << currentLevel);
        glDispatchCompute(nodesInCurrentLevel, 1, 1);
        
        // read new nodeCount
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        counterBuffer->getData(&nodeCount, sizeof(uint32));
    };
    
    // STAGE 2 - initialize next level
    // Current level is initialized and new uninitialized node tiles for this level are created
    // this stage will spawn a workgroup for each tile in current level with thread for each children in child node tile.
    // Each thread will zero the childs node data and calculate its center position.
    auto initializeLevel = [&](uint32 rootInit) {
        octreeInitiationProgram.use();
        octreeInitiationProgram.uniform("levelBeginIndex", currentLevelBeginIndex);
        octreeInitiationProgram.uniform("initRootTile", rootInit);
        RB_DEBUG("Initiating " << nodesInCurrentLevel << " nodes at level " << currentLevel << " (" << (nodesInCurrentLevel / nodesPerTile) << " tiles)");
        if (rootInit) {
            glDispatchCompute(1, 1, 1); // one workgroup for first tile
        } else {
            glDispatchCompute(nodesInCurrentLevel, 1, 1);
        }
    };
    
    // This is procedure which make sure that there is enough allocated storage in the SVO buffers for newly created nodes
    auto checkBufferStorageSizes = [&]() {
        auto resizeBuffer = [](gl::Buffer& buffer, uint32 requestedSize) {
            uint32 origSize = buffer.getSize();
            uint32 newSize = origSize;
            while(requestedSize > newSize) {
                newSize *= 1.5;
            }
            if (newSize != origSize) {
                RB_DEBUG("Buffer " << buffer.getGlID() << " reallocation: " << origSize << "->" << newSize);
                buffer.resize(newSize);
            }
        };
        resizeBuffer(*nodeBuffer, sizeof(uint32) * nodeCount);
        resizeBuffer(*vertexBuffer, sizeof(glm::vec4) * nodeCount);
    };
    
    // initiation of the first tile
    printLevel("Before any initiation");
    initializeLevel(1);
    printLevel("Current Level After initiation");

    // run the algorithm using procedures above
    while (nodesInCurrentLevel > 0 && currentLevel < maxSubdivisions) {
        
        evaluateLevel();
        checkBufferStorageSizes();
     
        printLevel("Current Level After evaluating");
        
        initializeLevel(0);
        
        // step into the next level
        printLevel("Current Level Initiation");
        
        // step into next level
        currentLevelBeginIndex = nodesInCurrentLevel + currentLevelBeginIndex;
        nodesInCurrentLevel    = nodeCount - currentLevelBeginIndex;
        currentLevel++;
        RB_DEBUG("Stepped into next level with " << (nodesInCurrentLevel / nodesPerTile) << " new node tiles (" << nodesInCurrentLevel << " nodes).");
    }
    
    printLevel("Final octree");
    RB_DEBUG("NODE count: " << nodeCount);
    
    vertexArray = make_unique<gl::VertexArray>();
    vertexArray->addAttrib(*vertexBuffer, 0, 4, GL_FLOAT);
}

void OctreeVT::prepare(const AppState& appState)
{
    // camera update
    auto cam = appState.cameraController->getCamera();
    if (cam.dirtyFlag) {
        program.loadStandardCamera(cam);
    }
}

void OctreeVT::render(const AppState& appState)
{
    program.use();
    vertexArray->bind();
    glDrawArrays(GL_POINTS, 0, glm::clamp(appState.drawBickCount, uint32(0), nodeCount));
    // glDrawArrays(GL_POINTS, 0, nodeCount);
}

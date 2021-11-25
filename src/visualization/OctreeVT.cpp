
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
    octreeWireFrameProgram(
        make_shared<gl::Shader>(GL_VERTEX_SHADER,   RESOURCE_SHADERS_NODE_VS),
        make_shared<gl::Shader>(GL_GEOMETRY_SHADER, RESOURCE_SHADERS_NODE_GS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_NODE_FS)
    ),
    octreeEvaluationProgram(
        make_shared<gl::Shader>(GL_COMPUTE_SHADER, RESOURCE_SHADERS_VSO_EVALUATE_BRICK_COMP, SHADER_MACROS)
    ),
    octreeInitiationProgram(
        make_shared<gl::Shader>(GL_COMPUTE_SHADER, RESOURCE_SHADERS_VSO_INIT_NEW_LEVEL_NODES_COMP, SHADER_MACROS)
    ),
    brickRenderProgram(
        make_shared<gl::Shader>(GL_VERTEX_SHADER,   RESOURCE_SHADERS_BRICK_VS),
        make_shared<gl::Shader>(GL_GEOMETRY_SHADER, RESOURCE_SHADERS_BRICK_GS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_BRICK_FS)
    )
{
}

void OctreeVT::prepare(const AppState& appState)
{
    if (!prepared) {
        prepared = true;
        
        // load list of edits on to GPU
        const Geometry& geometry = appState.geometryPool->getItem(0);
        
        // initial constant settings -> TODO: set this via gui
        const uint32 maxSubdivisions     = 5;
        const uint32 branchingFactor     = BRANCHING_FACTOR;
        const uint32 preallocatedNodes   = 10000;
        const uint32 nodesPerTile        = branchingFactor * branchingFactor * branchingFactor;
        const uint32 brickSize           = 8;
        const uint32 brickInOneDimension = 64;           // lets have 3d texture holding 64 x 64 x 64 bricks (512^3 voxels)
        const uint32 maximalBrickCount   = 64 * 64 * 64; // 262144 bricks - no additional bricks will be allocated
        const glm::vec4 correctionVector = glm::vec4(geometry.getAABB().center(), geometry.getAABB().longestEdgeSize()); // shift and scale SVO to be aligned with BB of the geometry
        
        
        // 3d texture for nodes preparation:
        // ---------------------------------

        volumeTexture = make_unique<gl::Texture3D>();
        glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        float borderDistance = 1.0f / glm::pow(BRANCHING_FACTOR, maxSubdivisions);
        glTextureParameterfv(volumeTexture->getGlID(), GL_TEXTURE_BORDER_COLOR, &borderDistance);
        glBindTexture(GL_TEXTURE_3D, volumeTexture->getGlID());
        glTexImage3D(
            GL_TEXTURE_3D,
            0,
            GL_RED,
            brickInOneDimension * brickSize,
            brickInOneDimension * brickSize,
            brickInOneDimension * brickSize,
            0,
            GL_RGBA,
            GL_FLOAT,
            nullptr
        );
        glBindImageTexture(
            0,                        // Texture unit
            volumeTexture->getGlID(), // Texture name
            0,                        // Level of Mip Map
            GL_TRUE,                  // Layered (false) -> this needs to be true because we have 3d texture
            0,                        // Specify layer if Layered is GL_FALSE
            GL_WRITE_ONLY,            // access
            GL_RGBA32F                // format
        );
        
            
        // Edit buffer preparation
        // ------------------------
        
        struct Edit {
            uint32    type;
            uint32    op;
            uint32    blending;
            uint32    _padding1;
            glm::vec3 pos;
            float     _padding2;
            glm::vec4 data;
        };
            
        std::vector<Edit> editsData;
        for (auto e : geometry.getEdits()) {
            editsData.push_back({
                e.primitiveType,
                e.operation,
                e.blending,
                0,
                e.transform.position,
                0,
                e.primitiveData
            });
        }
        editBuffer = make_unique<gl::Buffer>(editsData, GL_DYNAMIC_DRAW);
              
        
        // Octree NODE POOL initiation
        // ---------------------------
        
        nodeCount              = nodesPerTile; // one single initial tile
        brickCount             = 0;
        uint32 currentLevel    = 0;
        currentLevelBeginIndex = 0;
        nodesInCurrentLevel    = nodeCount;
        
        uint32 counters[2] = { nodeCount,  brickCount };
        counterBuffer = make_unique<gl::Buffer>(2 * sizeof(uint32), counters, GL_DYNAMIC_DRAW);
        nodeBuffer    = make_unique<gl::Buffer>(sizeof(uint32) * preallocatedNodes, nullptr, GL_DYNAMIC_DRAW);
        vertexBuffer  = make_unique<gl::Buffer>(sizeof(glm::vec4) * preallocatedNodes, nullptr, GL_DYNAMIC_DRAW);
        
        brickHelperBuffer = make_unique<gl::Buffer>(sizeof(glm::vec4) * 8 * 8 * 8, nullptr, GL_DYNAMIC_DRAW);
        
        // bind those buffers
        counterBuffer->bindBase(GL_ATOMIC_COUNTER_BUFFER, 0);
        nodeBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
        vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
        editBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);
        brickHelperBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 4);
        
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
        
        auto printBrick = [&]() {
            auto verticies = vector<glm::vec4>(8*8*8, glm::vec4(0));
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            brickHelperBuffer->getData(verticies);
            for (int i = 0; i < verticies.size(); i++) {
                RB_DEBUG("(" << i << ") " << glm::to_string(verticies[i]));
            }
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
        
        octreeEvaluationProgram.uniform("maxSubdivisions", maxSubdivisions);
        octreeEvaluationProgram.uniform("correctionVector", correctionVector);
        auto evaluateLevel = [&]() {
            octreeEvaluationProgram.use();
            octreeEvaluationProgram.uniform("levelBeginIndex", currentLevelBeginIndex);
            octreeEvaluationProgram.uniform("currentLevel", currentLevel);
            octreeEvaluationProgram.uniform("editCount", uint32(geometry.getEdits().size()));

            // one brick evaluated in one workgroup per tile
            RB_DEBUG("Evaluating " << nodesInCurrentLevel << " bricks at level " << currentLevel);
            glDispatchCompute(nodesInCurrentLevel, 1, 1);
            
            // read new nodeCount
            glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
            uint32 counterData[2];
            counterBuffer->getData(&counterData, 2 * sizeof(uint32));
            nodeCount  = counterData[0];
            brickCount = counterData[1];
        };
        
        // STAGE 2 - initialize next level
        // Current level is initialized and new uninitialized node tiles for this level are created
        // this stage will spawn a workgroup for each tile in current level with thread for each children in child node tile.
        // Each thread will zero the childs node data and calculate its center position.
        octreeInitiationProgram.uniform("correctionVector", correctionVector);
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
        // printLevel("Before any initiation");
        initializeLevel(1);
        // printLevel("Current Level After initiation");

        // run the algorithm using procedures above
        while (true) {
            
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            
            evaluateLevel();
            checkBufferStorageSizes();
            // printLevel("Current Level After evaluating");
            // printBrick();
            
            if (nodeCount == (nodesInCurrentLevel + currentLevelBeginIndex)) {
                break;
            }
            
            initializeLevel(0);

            // printLevel("Current Level Initiation");
            
            // step into next level
            currentLevelBeginIndex = nodesInCurrentLevel + currentLevelBeginIndex;
            nodesInCurrentLevel    = nodeCount - currentLevelBeginIndex;
            currentLevel++;
            RB_DEBUG("Stepped into level " << currentLevel << " with " << (nodesInCurrentLevel / nodesPerTile) << " new node tiles (" << nodesInCurrentLevel << " nodes).");
        }
        
        // printLevel("Final octree");
        RB_DEBUG("NODE count: "       << nodeCount);
        RB_DEBUG("Brick count: "       << brickCount);
        RB_DEBUG("Current level: "     << currentLevel);
        RB_DEBUG("Level begin index: " << currentLevelBeginIndex);
        RB_DEBUG("Nodes in level: "    << nodesInCurrentLevel);
        
        vertexArray = make_unique<gl::VertexArray>();
    }
    
    // camera update
    auto cam = appState.cameraController->getCamera();
    if (cam.dirtyFlag) {
        octreeWireFrameProgram.loadStandardCamera(cam);
        brickRenderProgram.loadStandardCamera(cam);
    }
}

void OctreeVT::render(const AppState& appState)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
    brickRenderProgram.use();
    vertexArray->bind();
    vertexArray->addAttrib(*vertexBuffer, 0, 4, GL_FLOAT, 0, sizeof(glm::vec4) * currentLevelBeginIndex);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // wait till texture is finished
    glDrawArrays(GL_POINTS, 0, nodesInCurrentLevel);
    
    // octreeWireFrameProgram.use();
    // vertexArray->bind();
    // vertexArray->addAttrib(*vertexBuffer, 0, 4, GL_FLOAT);
    // glDrawArrays(GL_POINTS, 0, nodeCount);

    // octreeWireFrameProgram.use();
    // vertexArray->bind();
    // vertexArray->addAttrib(*vertexBuffer, 0, 4, GL_FLOAT, 0, sizeof(glm::vec4) * currentLevelBeginIndex);
    // glDrawArrays(GL_POINTS, 0, nodesInCurrentLevel);
    
    // size = 8*8*8;
    // cnt = size; //glm::clamp(appState.drawBickCount, uint32(0), size);
    // vertexArray->addAttrib(*brickHelperBuffer, 0, 4, GL_FLOAT);
    // glDrawArrays(GL_POINTS, 0, cnt);
}

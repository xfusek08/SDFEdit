
#include <visualization/OctreeVT.h>

#include <RenderBase/defines.h>

#include <glm/glm.hpp>

#define GENERATE_POINT_VERTICIES
#ifdef GENERATE_POINT_VERTICIES
    #include <glm/gtx/string_cast.hpp>
    #define OCTREE_PROGRAM_DEFINES gl::ShaderDefines{"DEBUG", "GENERATE_POINT_VERTICIES"}
#else
    define OCTREE_PROGRAM_DEFINES gl::ShaderDefines{"DEBUG"}
#endif

using namespace std;
using namespace rb;

OctreeVT::OctreeVT() :
    program(
        make_shared<gl::Shader>(GL_VERTEX_SHADER,   RESOURCE_SHADERS_NODE_VS),
        make_shared<gl::Shader>(GL_GEOMETRY_SHADER, RESOURCE_SHADERS_NODE_GS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_NODE_FS)
    ),
    octreeProgram(
        make_shared<gl::Shader>(GL_COMPUTE_SHADER, RESOURCE_SHADERS_VSO_TOP_DOWN_CONSTRUCTION_COMP, OCTREE_PROGRAM_DEFINES)
    )
{
    // Octree construction testing
    // ---------------------------
    // NOTE: this should be done only once
    
    uint32 maxSubdivisions = 4;
    uint32 branchFactor    = 2;
    
    // node counter
    nodeCount = 8; // one single tile
    counterBuffer = make_unique<gl::Buffer>(sizeof(uint32), &nodeCount, GL_DYNAMIC_DRAW);
    
    // Calculate maximum node number given maximum subdivision
    // -------------------------------------------------------
    // Node pool has to be fully pre-allocated octree of depth 2 will contan maximum of 648 nodes
    //
    // Given that branching factor is 2 => 2^3 = 8 nodes per tile:
    //
    // level      tiles         nodes           TOTAL NODES
    // 0      |   1 = 8^0  |   1 * 8 =    8  |    8
    // 1      |   8 = 8^1  |   8 * 8 =   64  |    8 +   64 =   72
    // 2      |  64 = 8^2  |  64 * 8 =  512  |   72 +  512 =  584
    // 3      | 512 = 8^3  | 512 * 8 = 4096  |  584 + 4096 = 4680
    // --------------------------------------------------------
    //
    // For branching factor 2 recurrent equation for total nodes is:
    //      a(n) = (8^n) * 8 + a(n-1); a(0) = 8
    //
    // For arbitrary branching factor b the equation is:
    //      a(n) = ((b^3)^n) * (b^3) + a(n-1); a(0) = b^3
    //
    // The solved equation for n as max tree level and b as branching factor is:
    //      a(n) = (b^3 ((b^3)^(n + 1) - 1))/(b^3 - 1)
    //
    //      see: https://www.wolframalpha.com/input/?i=a%28n%29+%3D+%28%28b%5E3%29%5En%29+*+%28b%5E3%29+%2B+a%28n-1%29%3B+a%280%29+%3D+b%5E3
    //
    glm::f32 n = maxSubdivisions;
    glm::f32 b3 = glm::pow(branchFactor, 3);
    uint32 nodesNumber = (b3 * (glm::pow(b3, n + 1) - 1)) / (b3 - 1);
    nodeBuffer = make_unique<gl::Buffer>(sizeof(uint32) * nodesNumber, nullptr, GL_DYNAMIC_DRAW);
    
    #ifdef GENERATE_POINT_VERTICIES
        vertexBuffer = make_unique<gl::Buffer>(sizeof(glm::vec4) * nodesNumber, nullptr, GL_DYNAMIC_DRAW);
        vector<glm::vec4> firstTileVerticies = {
            glm::vec4(-0.5,  0.5,  0.5, 0.5),
            glm::vec4( 0.5,  0.5,  0.5, 0.5),
            glm::vec4(-0.5, -0.5,  0.5, 0.5),
            glm::vec4( 0.5, -0.5,  0.5, 0.5),
            glm::vec4(-0.5,  0.5, -0.5, 0.5),
            glm::vec4( 0.5,  0.5, -0.5, 0.5),
            glm::vec4(-0.5, -0.5, -0.5, 0.5),
            glm::vec4( 0.5, -0.5, -0.5, 0.5)
        };
        vertexBuffer->setData(firstTileVerticies);
    #endif

    auto printNodeBuffer = [&](const char* caption) {
        auto nodes = vector<uint32>(nodeCount, 0);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        nodeBuffer->getData(nodes);
        
        #ifdef GENERATE_POINT_VERTICIES
            auto verticies = vector<glm::vec4>(nodeCount, glm::vec4(0));
            vertexBuffer->getData(verticies);
            #define ADD_VERTEX_INFO(I) << "    " << glm::to_string(verticies[I])
        #else
            #define ADD_VERTEX_INFO(I)
        #endif
        
        RB_DEBUG(caption << ":");
        RB_DEBUG("  Tiles:");
        for (int tileIndex = 0; tileIndex < nodeCount / 8; ++tileIndex) {
            RB_DEBUG("    Tile: " << tileIndex);
            for (int localIndex = 0; localIndex < 8; ++localIndex) {
                int i = tileIndex * 8 + localIndex;
                RB_DEBUG("      (" << i << ") " << localIndex << ":  " <<
                    ((nodes[i] & 0x80000000) ? "1" : "0") <<
                    " | " <<((nodes[i] & 0x40000000) ? "1" : "0") <<
                    " | " << (nodes[i] & 0x3FFFFFFF) ADD_VERTEX_INFO(i)
                );
            }
        }
        RB_DEBUG(" ");
    };
    
    // Current octree level is:
    //    begining index (first node of this level)
    //    number of nodes in this level
    struct CurrentSVOLevel {
        uint32 beginNodeIndex;
        uint32 nodesInLevel;
    };
    auto printLevel = [](const char* caption, CurrentSVOLevel level) {
        RB_DEBUG(caption << "\n" <<
            "    beginNodeIndex: " << level.beginNodeIndex << "\n"
            "      nodesInLevel: " << level.nodesInLevel
        );
    };
    
    // STEP 1 Flagging for subdivision step
    auto flagNodes = [&]() {
        octreeProgram.uniform("levelBeginIndex", 0);
        octreeProgram.uniform("stage", uint32(0));
        glDispatchCompute(nodeCount / 8, 1, 1);
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    };
    
    
    // STEP 2 dispatch only over current level of nodes
    //        for each node which is flagged as "to be subdivided"
    //        allocate new node -> new level is created with only uninitialized node
    auto subdivideLevel = [&](CurrentSVOLevel level) {
        octreeProgram.uniform("levelBeginIndex", level.beginNodeIndex);
        octreeProgram.uniform("stage", uint32(1));
        glDispatchCompute(level.nodesInLevel / 8, 1, 1);
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        counterBuffer->getData(&nodeCount, sizeof(uint32));
        return CurrentSVOLevel{
            level.nodesInLevel + level.beginNodeIndex,
            nodeCount - (level.nodesInLevel + level.beginNodeIndex),
        };
    };
    
    
    // STEP 3 launch ini shader for new node level and null all this nodes
    auto initCurrentLevel = [&](CurrentSVOLevel level) {
        octreeProgram.uniform("levelBeginIndex", level.beginNodeIndex);
        octreeProgram.uniform("stage", uint32(2));
        glDispatchCompute(level.nodesInLevel / 8, 1, 1);
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        return level;
    };
    
    // prepare program
    octreeProgram.use();
    counterBuffer->bindBase(GL_ATOMIC_COUNTER_BUFFER, 0);
    nodeBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    octreeProgram.uniform("maxSubdivisionLevel", maxSubdivisions);
    #ifdef GENERATE_POINT_VERTICIES
        vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
    #endif
    
    // top down algorithm
    CurrentSVOLevel currentLevel = initCurrentLevel({ 0, nodeCount});
    // printLevel("initial level", currentLevel);
    // printNodeBuffer("After init dispatch");
    
    for (int i = 0; i < maxSubdivisions; ++i) {
        flagNodes();
     
        printLevel("Current Level After Flagging", currentLevel);
        printNodeBuffer("After flagging");
     
        currentLevel = subdivideLevel(currentLevel);
        
        if (currentLevel.nodesInLevel == 0) {
            break;
        }

        printLevel("Current Level After subdivision", currentLevel);
        printNodeBuffer("After subdivision");
        
        currentLevel = initCurrentLevel(currentLevel);
        
        printLevel("Current Level After initiation", currentLevel);
        printNodeBuffer("After initiation");
    }
    printNodeBuffer("Final octree");
    RB_DEBUG("NODE count: " << nodeCount);
    vertexArray = make_unique<gl::VertexArray>();
    vertexArray->addAttrib(*vertexBuffer, 0, 4, GL_FLOAT);
}

void OctreeVT::prepare(const AppState& appState)
{
}

void OctreeVT::render(const AppState& appState)
{
    // camera update
    auto cam = appState.cameraController->getCamera();
    if (cam.dirtyFlag) {
        program.loadStandardCamera(cam);
    }
    
    
    // TODO: update vertex buffer when model is dirty
    
    // tmp debug implementation
    // ------------------
    
    program.use();
    vertexArray->bind();
    glDrawArrays(GL_POINTS, 0, nodeCount);
}

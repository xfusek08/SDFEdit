
#include <data/SVOctree.h>

#ifdef DEBUG
    #include <RenderBase/logging.h>
    #include <glm/gtx/string_cast.hpp>
#endif

#include <vector>

using namespace std;
using namespace rb;

SVOctree::SVOctree(uint32 branchingFactor, GLsizeiptr initialSize) :
    branchingFactor(branchingFactor)
{
    allocate(initialSize);
    brickPool = make_unique<BrickPool>();
}

void SVOctree::allocate(GLsizeiptr size)
{
    if (!isInitialized()) {
        nodeBuffer     = make_unique<gl::Buffer>(sizeof(uint32) * size, nullptr, GL_DYNAMIC_DRAW);
        nodeDataBuffer = make_unique<gl::Buffer>(sizeof(uint32) * size, nullptr, GL_DYNAMIC_DRAW);
        vertexBuffer   = make_unique<gl::Buffer>(sizeof(glm::vec4) * size, nullptr, GL_DYNAMIC_DRAW);
    } else {
        nodeBuffer->resize(sizeof(uint32) * size);
        nodeDataBuffer->resize(sizeof(uint32) * size);
        vertexBuffer->resize(sizeof(glm::vec4) * size);
    }
    allocated = size;
}

SVOctree::Level* SVOctree::addLevel(Level level)
{
    levels.push_back(level);
    return &(levels[levels.size() - 1]);
}

SVOctree::Level* SVOctree::initFirstLevel()
{
    nodeCount = getNodesPerTile();
    brickPool->brickCount = 0;
    levels.clear();
    
    Level initialLevel = {};
    initialLevel.nodeCount = nodeCount;
    return addLevel(initialLevel);
}

#ifdef DEBUG
    void SVOctree::debugPrint() const
    {
        auto nodes     = vector<uint32>(nodeCount, 0u);
        auto nodeData  = vector<uint32>(nodeCount, 0u);
        auto verticies = vector<glm::vec4>(nodeCount, glm::vec4(0.f));
        
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        nodeDataBuffer->getData(nodeData);
        nodeBuffer->getData(nodes);
        vertexBuffer->getData(verticies);

        RB_DEBUG("Octree on GPU:");
        RB_DEBUG("  Node count:   " << nodeCount);
        RB_DEBUG("  Allocated:    " << allocated);
        RB_DEBUG("  Depth:        " << getDepth());
        RB_DEBUG("  Levels:");
        for (auto level : levels) {
            RB_DEBUG("    " << level.depth << ": ");
            RB_DEBUG("      Start node:  " << level.startNode);
            RB_DEBUG("      End node:    " << level.startNode + level.nodeCount);
            RB_DEBUG("      Node count:  " << level.nodeCount);
            RB_DEBUG("      Brick count: " << level.bricksInLevel);
        }
        
        RB_DEBUG("  Tiles:");
        for (int tileIndex = 0; tileIndex < (nodeCount / getNodesPerTile()); ++tileIndex) {
            RB_DEBUG("    Tile: " << tileIndex);
            for (int localIndex = 0; localIndex < getNodesPerTile(); ++localIndex) {
                int i = tileIndex * getNodesPerTile() + localIndex;
                RB_DEBUG("      (" << i << ") " << localIndex << ":  " <<
                    ((nodes[i] & 0x80000000) ? "1" : "0") <<
                    " | " <<((nodes[i] & 0x40000000) ? "1" : "0") <<
                    " | " << (nodes[i] & 0x3FFFFFFF) <<
                    "   " << nodeData[i] <<
                    "   " << glm::to_string(verticies[i])
                );
            }
        }
        RB_DEBUG(" ");
    }
#endif

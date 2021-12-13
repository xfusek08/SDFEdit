
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

vector<SVOctree::Node> SVOctree::getNodes() const
{
    auto res = vector<SVOctree::Node>(nodeCount, { 0 });
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    nodeBuffer->getData(res);
    return move(res);
}

#ifdef DEBUG
    void printLevel(const SVOctree::Level& level) {
        RB_DEBUG("    " << level.depth << ": ");
        RB_DEBUG("      Start node:  " << level.startNode);
        RB_DEBUG("      End node:    " << level.startNode + level.nodeCount);
        RB_DEBUG("      Node count:  " << level.nodeCount);
        RB_DEBUG("      Brick count: " << level.bricksInLevel);
    }
    
    void SVOctree::debugPrint() const
    {
        auto nodes     = getNodes();
        auto nodeData  = vector<uint32>(nodeCount, 0u);
        auto verticies = vector<glm::vec4>(nodeCount, glm::vec4(0.f));
        
        nodeDataBuffer->getData(nodeData);
        vertexBuffer->getData(verticies);

        RB_DEBUG("Octree on GPU:");
        RB_DEBUG("  Node count:   " << nodeCount);
        RB_DEBUG("  Allocated:    " << allocated);
        RB_DEBUG("  Depth:        " << getDepth());
        RB_DEBUG("  Levels:");
        for (auto i = levels.begin(); i != levels.end(); ++i){
            const auto& level = *i;
            printLevel(level);
        
            RB_DEBUG("      Tiles:");
            for (int tileIndex = level.startNode / getNodesPerTile(); tileIndex < ((level.startNode + level.nodeCount) / getNodesPerTile()); ++tileIndex) {
                RB_DEBUG("        Tile: " << tileIndex);
                for (int localIndex = 0; localIndex < getNodesPerTile(); ++localIndex) {
                    int i = tileIndex * getNodesPerTile() + localIndex;
                    auto node = nodes[i];
                    RB_DEBUG("          (" << i << ") " << localIndex << ":  " <<
                        (node.isDivided() ? "d" : " ") <<
                        " | " << (node.hasBrick() ? "b" : " ") <<
                        " | " << node.getTileIndex() <<
                        "   " << (node.hasBrick() ? STREAM_TO_STR(nodeData[i]) : ((nodeData[i] == 1) ? "s" : "e")) <<
                        "   " << glm::to_string(verticies[i])
                    );
                }
            }
        }
        RB_DEBUG(" ");
    }
    
    void SVOctree::debugPrintLevels() const
    {
        for (const auto& level : levels) {
            printLevel(level);
        }
    }
    
#endif

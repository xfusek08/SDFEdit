#pragma once

#include <data/BrickPool.h>

#include <RenderBase/defines.h>
#include <RenderBase/gl/Buffer.h>

#include <memory>
#include <vector>


struct SVOctree {
    
    struct Level {
        uint32 startNode     = 0;
        uint32 nodeCount     = 0;
        uint32 depth         = 0;
        uint32 bricksInLevel = 0;
    };

    // metadata
    GLsizeiptr         nodeCount       = 0;
    uint32             branchingFactor = 0;
    std::vector<Level> levels          = {};
    
    // gpu data
    std::unique_ptr<rb::gl::Buffer> nodeBuffer     = nullptr;
    std::unique_ptr<rb::gl::Buffer> nodeDataBuffer = nullptr;
    std::unique_ptr<rb::gl::Buffer> vertexBuffer   = nullptr;
    std::unique_ptr<BrickPool>      brickPool      = nullptr;
    
    // methods
    SVOctree(uint32 branchingFactor, GLsizeiptr initialSize);
    
    void   allocate(GLsizeiptr size);
    Level* addLevel(Level level);
    Level* initFirstLevel();
    
    inline bool       isInitialized()    const { return nodeBuffer != nullptr && nodeDataBuffer != nullptr && vertexBuffer != nullptr; }
    inline GLsizeiptr getAllocatedSize() const { return allocated; }
    inline uint32     getDepth()         const { return levels.size(); }
    inline uint32     getNodesPerTile()  const { return branchingFactor * branchingFactor * branchingFactor; }
    
    #ifdef DEBUG
        void debugPrint() const;
    #endif
    
    private:
        GLsizeiptr allocated = 0;
};

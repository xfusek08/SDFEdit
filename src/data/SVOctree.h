#pragma once

#include <RenderBase/defines.h>
#include <RenderBase/gl/Buffer.h>

#include <memory>
#include <vector>

struct SVOLevel {
    uint32 startNode;
    uint32 nodeCount;
    uint32 depth;
};

struct SVOctree
{
    uint32 size = 0;
    
    std::unique_ptr<rb::gl::Buffer> nodeBuffer;
    std::unique_ptr<rb::gl::Buffer> nodeDataBuffer;
    std::unique_ptr<rb::gl::Buffer> vertexBuffer;
    std::vector<SVOLevel> levels; // NOTE: metadata of the octree to help with rendering
};

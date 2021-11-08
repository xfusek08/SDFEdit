
#pragma once

#include <RenderBase/gl/Buffer.h>
#include <RenderBase/gl/Texture3D.h>

struct EvaluatedGeometry
{
    std::unique_ptr<rb::gl::Buffer> editsBuffer;
    std::unique_ptr<rb::gl::Texture3D> volumeTexture; // TODO: template with octree node buffer and use single shared sparse volumetric texture to store bricks from all the geometries
    
    // NOTE: uniforms about this evaluated geometry -- will change when we stop using single 3D texture
    uint32  voxelCount = 0;
    float32 voxelSize  = 0;
};

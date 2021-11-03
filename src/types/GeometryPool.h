
#pragma once

#include <types/scene.h>

#include <types/ElementPoolBehavior.h>

#include <glad/glad.h>

struct EvaluatedGeometry {
    // TODO: new buffer with node tree
    
    uint32  voxelCount;
    float32 voxelSize;
    GLuint  volumeTextrue;
};

// TODO: maybe avoid std::vector all togeater as future optimization and hanfle the memory myself inside pre-alllocated fixed chunks
// to me re-evaluation faster without anny mallocs
struct GeometryPool : public ItemPoolBehavior<Geometry> {
    // TODO: manipulation with individual geometries will be provided by interface to ensure keeping track of
    
    GeometryPool() {}
    
    ~GeometryPool() {} // TODO: destoy evaluated geometries on the GPU

    // TODO: + array of buffers one for each geometry node tree
    // TODO: buffer (sparse 3D texture) of brick bool shared by all the geometries
    
    // NOTE: temp implementation with single flat 3d texture
    std::vector<EvaluatedGeometry> evaluatedGeometries;
};

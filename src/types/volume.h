/**
 * this files defines volumetric data structure which will be eventually moved entirely to the GPU
 */

#pragma once

#include <RenderBase/defines.h>
#include <RenderBase/constants.h>
#include <RenderBase/asserts.h>
#include <glm/glm.hpp>

#include <memory>

/**
 * Data stored per voxel (on CPU for now)
 */
struct VoxelData {
    glm::f32 sdfValue = FLT_MAX;
    // glm::vec3 normal; // later on use normal and encode sdf value into its length
    glm::vec3 color = rb::colors::white;
};

// TODO extend to templated general data structure "Buffer3D<VoxelData>"
class Volume {
    public:
        Volume(glm::f32 voxelSize, glm::u32 voxelCount) :
            voxelSize(voxelSize),
            voxelCount(voxelCount),
            voxels(std::unique_ptr<VoxelData[]>(new VoxelData[voxelCount*voxelCount*voxelCount]))
            {}
            
        inline glm::f32 getVoxelSize()  const { return voxelSize; }
        inline glm::u32 getVoxelCount() const { return voxelCount; }
        
        inline VoxelData getVoxel(uint32 x, uint32 y, uint32 z) {
            RB_ASSERT(x < voxelCount);
            RB_ASSERT(y < voxelCount);
            RB_ASSERT(z < voxelCount);
            return voxels[z*voxelCount*voxelCount + y*voxelCount + x];
        };
        
        inline void setVoxel(uint32 x, uint32 y, uint32 z, VoxelData data) {
            RB_ASSERT(x < voxelCount);
            RB_ASSERT(y < voxelCount);
            RB_ASSERT(z < voxelCount);
            voxels[z*voxelCount*voxelCount + y*voxelCount + x] = data;
        };
        
    private:
        glm::f32 voxelSize = 0;  // This is a number describing a edge size of one voxel
        glm::u32 voxelCount = 0; // A discrete number of voxel in one dimension (N)  thus data will be allways NxNxN voxels
        std::unique_ptr<VoxelData[]> voxels = nullptr;
};

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
    glm::vec4 data = glm::vec4(0);
    
    inline void setNormal(glm::vec3 value) {  data.x = value.x; data.y = value.y; data.x = value.z; }
    inline glm::vec3 getNormal() const { return glm::vec3(data); }
    
    inline void setSDFVal(glm::f32 value) {  data.a = value; }
    inline glm::f32 getSDFVal() const { return data.a; }
};

// TODO this will be implemented in compute shader directly writing into a 3D texture
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
        
        inline VoxelData* getRawDataPointer() { return voxels.get(); }
    
    private:
        glm::f32 voxelSize = 0;  // This is a number describing a edge size of one voxel
        glm::u32 voxelCount = 0; // A discrete number of voxel in one dimension (N)  thus data will be allways NxNxN voxels
        std::unique_ptr<VoxelData[]> voxels = nullptr;
};

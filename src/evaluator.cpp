
#include <evaluator.h>
#include <types/primitives.h>

#include <RenderBase/logging.h>

using namespace primitives;
using namespace std;

#define BOUNDING_OFFSET 0.5f

glm::f32 distanceToEdit(const GeometryEdit& edit, const glm::vec3& coords, const glm::vec3& centerCorrection)
{
    switch (edit.primitiveType)
    {
        case PrimitiveType::ptSphere: {
            auto c = Sphere::center(edit) + centerCorrection;
            auto d = glm::distance(c, coords);
            auto r = Sphere::radius(edit);
            auto dr = d - r;
            return dr;
        }
        default:
        return FLT_MAX;
    }
}

Volume* buildVolumeForGeometry(const Geometry& geometry)
{
    // We want to fit the volume cube to the center of geometry bounding box with some space around.
    glm::f32 minimalVolumeEdgeLength = geometry.getAABB().longestEdgeSize() + BOUNDING_OFFSET;
    
    // Unit cube is divided into geometry.resolution^3 cubical voxels.
    glm::f32 voxelSize = 1.0F / geometry.getResolution();
    
    // nearest upper even number of voxel in one edge of volume
    glm::u32 voxelCount = glm::round(glm::ceil(minimalVolumeEdgeLength / voxelSize) / 2.0f) * 2.0;
    
    // primitives inside geometry will be displaced relative to the center of BB the insted of geometry origin.
    glm::vec3 primitiveCenterCorrection = -geometry.getAABB().center();
    
    // axis-paralel distance from center to the center OF edge voxel.
    // for example voxelCount = 4 and voxel size (v) is 1:
    //     then there is 2 voxels on each half and center of the last voxel is 1.5 from the center of the edge
    glm::f32 maxVoxelCenterCoord = ((voxelCount - 1) * voxelSize) * 0.5f;
    
    auto volume = new Volume(voxelSize, voxelCount);
    
    // eachVoxel
    for (uint32 z = 0; z < voxelCount; ++z) {
        for (uint32 y = 0; y < voxelCount; ++y) {
            for (uint32 x = 0; x < voxelCount; ++x) {
                
                // compute position of center of current voxel relative to the origin
                glm::vec3 voxelPosition = {
                    -maxVoxelCenterCoord + x * voxelSize,
                    -maxVoxelCenterCoord + y * voxelSize,
                    -maxVoxelCenterCoord + z * voxelSize,
                };
                
                VoxelData voxel = volume->getVoxel(x,y,z);
                
                glm::f32 sdfValue = voxel.getSDFVal();
                for (const auto& edit : geometry.getEdits()) {
                    sdfValue = glm::min(
                        distanceToEdit(edit, voxelPosition, primitiveCenterCorrection),
                        sdfValue
                    );
                    // TODO: calculate normal and evaluate distance into its length
                }
                voxel.setSDFVal(sdfValue);
                volume->setVoxel(x,y,z, voxel);
            }
        }
    }
    return volume;
}

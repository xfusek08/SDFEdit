#ifdef DEBUG
    // #define NO_LOG // uncomment if debug logging should be ignored for this file
    #include <RenderBase/logging.h>
    #include <glm/gtx/string_cast.hpp>
#endif

#include <evaluator.h>
#include <types/primitives.h>

using namespace primitives;
using namespace std;

#define BOUNDING_OFFSET 0.0f

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

unique_ptr<Volume> evaluator::buildVolumeForGeometry(const Geometry& geometry)
{
    // Nearest upper even number of voxel in one edge of volume.
    glm::u32 voxelCount = glm::round(glm::f32(geometry.getResolution()) / 2.0f) * 2.0f;
    
    RB_DEBUG("geometry.getAABB().longestEdgeSize(): " << geometry.getAABB().longestEdgeSize());
    
    // Unit cube around the BB is divided into geometry.resolution^3 cubical voxels.
    glm::f32 voxelSize = (geometry.getAABB().longestEdgeSize() + BOUNDING_OFFSET) / glm::f32(voxelCount - 2);
    
    // Primitives inside geometry will be displaced relative to the center of BB the insted of geometry origin.
    glm::vec3 primitiveCenterCorrection = -geometry.getAABB().center();
    
    // Axis-paralel distance from center to the center OF edge voxel.
    // For example voxelCount = 4 and voxelSize is 1 (4x4x4 cube):
    //     There are 2 voxels in each direction from the origin and center of the last voxel is 1.5 away in the direction
    glm::f32 maxVoxelCenterCoord = ((voxelCount - 1) * voxelSize) * 0.5f;
    
    // Lets create the volume
    auto volume = make_unique<Volume>(voxelSize, voxelCount);
    
    // for each voxel (compute shader kernel)
    for (uint32 z = 0; z < voxelCount; ++z) {
        for (uint32 y = 0; y < voxelCount; ++y) {
            for (uint32 x = 0; x < voxelCount; ++x) {
                
                // compute position of center of current voxel relative to the origin
                glm::vec3 voxelPosition = {
                    -maxVoxelCenterCoord + x * voxelSize,
                    -maxVoxelCenterCoord + y * voxelSize,
                    -maxVoxelCenterCoord + z * voxelSize,
                };
                
                // find sdf value and closest primitive
                glm::f32 sdfValue      = FLT_MAX;
                uint32   closestEditId = 0;
                uint32   actEditId     = 0;
                for (const GeometryEdit& edit : geometry.getEdits()) {
                    glm::f32 primitiveDist = distanceToEdit(edit, voxelPosition, primitiveCenterCorrection);
                    if (primitiveDist < sdfValue) {
                        sdfValue = primitiveDist;
                        closestEditId = actEditId;
                    }
                    ++actEditId;
                }
                
                // compute normal
                glm::f32 e = voxelSize * 0.1f;
                glm::vec3 normal = glm::normalize(sdfValue - glm::vec3(
                    distanceToEdit(geometry.getEdits()[closestEditId], voxelPosition - glm::vec3(e, 0, 0), primitiveCenterCorrection),
                    distanceToEdit(geometry.getEdits()[closestEditId], voxelPosition - glm::vec3(0, e, 0), primitiveCenterCorrection),
                    distanceToEdit(geometry.getEdits()[closestEditId], voxelPosition - glm::vec3(0, 0, e), primitiveCenterCorrection)
                ));

                // set voxel
                VoxelData voxel = volume->getVoxel(x,y,z);
                voxel.setNormal(normal);
                voxel.setSDFVal(sdfValue);
                volume->setVoxel(x, y, z, voxel);
            }
        }
    }
    return move(volume);
}

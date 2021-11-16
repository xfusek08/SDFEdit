
#pragma once

#include <data/AABB.h>
#include <data/Transform.h>
#include <data/ItemPoolBehavior.h>

#include <RenderBase/defines.h>
#include <RenderBase/constants.h>

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#define DEFAULT_GEOMETRY_RESOLUTION 8

using PrimitiveData = glm::vec4;

enum PrimitiveType {
    ptSphere    = 0,
    ptCapsule   = 1,
    ptTorus     = 2,
    ptBox       = 3,
    ptCylinder  = 4,
    ptCone      = 5,
    ptRoundCone = 6,
};

enum GeometryOperation {
    opAdd       = 0,
    opSubstract = 1,
    opIntersect = 2,
    opPaint     = 3,
};

struct GeometryEdit {
    glm::u32 primitiveType;
    glm::u32 operation;
    glm::u32 blending;
    Transform transform;     // 7x float TODO: maybe store transform as 4x4 matrix?
    glm::vec4 color;         // 3x float - color value TODO: maybe pack it to 3 bytes (255 ber chanel)
    glm::vec4 primitiveData; // 4x float TODO: maybe store as union depending on primitive type, I dont know how, much data will more complex shapes have to hold.
    
    // TODO: first 3 properties could be packed into a single integer
};

struct Geometry {
    /**
     * Resolution means imply the number of voxels along one edge ot the volume cube.
     * So its refering to the 'N' of the N x N x N volume.
     */
    glm::u32 resolution;
    
    // Methods for encapsulated access to data
    // ---------------------------------------
    
    Geometry(glm::f32 resolution = DEFAULT_GEOMETRY_RESOLUTION) : resolution(resolution) {}
    
    void addEdit(GeometryEdit edit);
    void addEdits(const std::vector<GeometryEdit>& edits);
    void clearEdits();
    
    inline const std::vector<GeometryEdit>& getEdits() const { return edits; }
    inline glm::u32 getResolution() const { return resolution; }
    inline AABB getAABB() const { return aabb; }
    
    private:
        /**
         * List of edits creating this geometry
         */
        std::vector<GeometryEdit> edits = {};
        
        /**
         * Axis Aligned Bounding Box for the whole geometry, it will be useful when computing origin and aligning volume data.
         */
        AABB aabb = AABB();
};

// TODO: maybe avoid std::vector all togeater as future optimization and hanfle the memory myself inside pre-alllocated fixed chunks
// to me re-evaluation faster without anny mallocs
struct GeometryPool : public ItemPoolBehavior<Geometry>
{
};

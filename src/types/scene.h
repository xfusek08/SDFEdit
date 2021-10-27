
#pragma once

#include <types/AABB.h>
#include <types/transform.h>

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
    glm::u8   primitiveType; // 255 primitives is still way to much
    glm::u8   operation;     // 255 operations is still way to much
    Transform transform;
    glm::vec3 color;
    glm::f32  blending;
    glm::vec4 primitiveData;
};

class Geometry {
    public:
        Geometry(glm::f32 resolution = DEFAULT_GEOMETRY_RESOLUTION) :
            resolution(resolution) {}
        
        void addEdit(GeometryEdit edit);
        void addEdits(const std::vector<GeometryEdit>& edits);
        
        
        inline const std::vector<GeometryEdit>& getEdits() const { return edits; }
        inline glm::u32 getResolution() const { return resolution; }
        inline AABB getAABB() const { return aabb; }
        
    private:
        /**
         * List of edits creating this geometry
         */
        std::vector<GeometryEdit> edits = {};
        
        /**
         * Resolution means imply the number of voxels along one edge ot the volume cube.
         * So its refering to the 'N' of the N x N x N volume.
         */
        glm::u32 resolution;
        
        /**
         * Axis Aligned Bounding Box for the whole geometry, it will be useful when computing origin and aligning volume data.
         */
        AABB aabb = AABB();
};

struct Model {
    glm::u32  geometryId;
    Transform transform;
};

struct Scene {
    std::vector<Geometry> geometryPool = {};
    std::vector<Model>    modelPool    = {};
};

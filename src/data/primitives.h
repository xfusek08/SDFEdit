
#pragma once

#include <data/geometry.h>

// specific operation for particular primitive types
namespace primitives {
    namespace Sphere {
        inline glm::vec3 center(const GeometryEdit& edit)     { return edit.transform.position; }
        inline glm::f32  radius(const GeometryEdit& edit)     { return edit.primitiveData.x; }
        inline glm::vec3 dimensions(const GeometryEdit& edit) { return 2.0f * glm::vec3(radius(edit)) + (edit.blending) * 0.5f; }
        
        inline void setRadius(GeometryEdit& edit, float32 radius) { edit.primitiveData.x = radius; }
        
        inline GeometryEdit createEdit(
            glm::u8   operation = GeometryOperation::opAdd,
            Transform transform = {},
            glm::f32  radius    = 0.5f,
            glm::vec3 color     = rb::colors::white,
            glm::f32  blending  = 0.0f
        ) {
            GeometryEdit res;
            res.primitiveType = PrimitiveType::ptSphere;
            res.operation     = operation;
            res.blending      = glm::clamp(blending, 0.0001f, 1.0f);
            res.color         = glm::vec4(color, 1);
            res.transform     = transform;
            res.primitiveData = { radius, 0, 0, 0 };
            return res;
        }
    };
    
    namespace Capsule {
        inline float32   width(const GeometryEdit& edit)      { return 2.0 * edit.primitiveData.x; }
        inline float32   height(const GeometryEdit& edit)     { return edit.primitiveData.y; }
        inline glm::vec3 dimensions(const GeometryEdit& edit) { return glm::vec3(width(edit), height(edit), width(edit)) + (edit.blending * 0.5f); }
        
        inline GeometryEdit createEdit(
            glm::u8   operation = GeometryOperation::opAdd,
            Transform transform = {},
            float32   width     = 0.5,
            float32   height    = 1.0,
            glm::vec3 color     = rb::colors::white,
            glm::f32  blending  = 0.0f
        ) {
            GeometryEdit res;
            res.primitiveType = PrimitiveType::ptCapsule;
            res.operation     = operation;
            res.blending      = glm::clamp(blending, 0.0001f, 1.0f);
            res.color         = glm::vec4(color, 1);
            res.transform     = transform;
            res.primitiveData = { 0.5 * width, height, 0, 0 };
            return res;
        }
    };
    
    namespace Torus {
        inline glm::vec3 dimensions(const GeometryEdit& edit) { return glm::vec3(edit.primitiveData.x + edit.primitiveData.y, edit.primitiveData.y, edit.primitiveData.x + edit.primitiveData.y) * 2.0f + (edit.blending * 0.5f); }
        
        inline GeometryEdit createEdit(
            glm::u8   operation   = GeometryOperation::opAdd,
            Transform transform   = {},
            float32   outerRadius = 0.5,
            float32   innerRadius = 0.25,
            glm::vec3 color       = rb::colors::white,
            glm::f32  blending    = 0.0f
        ) {
            GeometryEdit res;
            res.primitiveType = PrimitiveType::ptTorus;
            res.operation     = operation;
            res.blending      = glm::clamp(blending, 0.0001f, 1.0f);
            res.color         = glm::vec4(color, 1);
            res.transform     = transform;
            res.primitiveData = { outerRadius, innerRadius, 0, 0 };
            return res;
        }
    };
    
    namespace Box {
        inline glm::vec3 dimensions(const GeometryEdit& edit) { return 2.0f * glm::vec3(edit.primitiveData) + (edit.blending * 0.5f); }
        
        inline GeometryEdit createEdit(
            glm::u8   operation  = GeometryOperation::opAdd,
            Transform transform  = {},
            glm::vec3 dimensions = glm::vec3(1.0f),
            float32   rounding   = 0.0f,
            glm::vec3 color      = rb::colors::white,
            glm::f32  blending   = 0.0f
        ) {
            GeometryEdit res;
            res.primitiveType = PrimitiveType::ptBox;
            res.operation     = operation;
            res.blending      = glm::clamp(blending, 0.0001f, 1.0f);
            res.color         = glm::vec4(color, 1);
            res.transform     = transform;
            res.primitiveData = glm::vec4(0.5f * dimensions, rounding);
            return res;
        }
    };
    
    namespace Cylinder {
        inline glm::vec3 dimensions(const GeometryEdit& edit) {
            return 2.0f * glm::vec3(
                edit.primitiveData.x,
                edit.primitiveData.y,
                edit.primitiveData.x
            ) + (edit.blending * 0.5f);
        }
        
        inline GeometryEdit createEdit(
            glm::u8   operation = GeometryOperation::opAdd,
            Transform transform = {},
            float32   height    = 1.0,
            float32   radius    = 0.5,
            float32   rounding  = 0.0,
            glm::vec3 color     = rb::colors::white,
            glm::f32  blending  = 0.0f
        ) {
            GeometryEdit res;
            res.primitiveType = PrimitiveType::ptCylinder;
            res.operation     = operation;
            res.blending      = glm::clamp(blending, 0.0001f, 1.0f);
            res.color         = glm::vec4(color, 1);
            res.transform     = transform;
            res.primitiveData = { radius, 0.5 * height, rounding, 0 };
            return res;
        }
    };
    
    namespace Cone {
        inline glm::vec3 dimensions(const GeometryEdit& edit)
        {
            return glm::vec3(
                2.0f * glm::max(edit.primitiveData.x, edit.primitiveData.y),
                edit.primitiveData.z,
                2.0f * glm::max(edit.primitiveData.x, edit.primitiveData.y)
            ) + (edit.blending * 0.5f);
        }
        
        inline GeometryEdit createEdit(
            glm::u8   operation    = GeometryOperation::opAdd,
            Transform transform    = {},
            float32   radiusTop    = 0.25,
            float32   radiusBottom = 0.5,
            float32   height       = 1.0,
            float32   rounding     = 0.0,
            glm::vec3 color        = rb::colors::white,
            glm::f32  blending     = 0.0f
        ) {
            GeometryEdit res;
            res.primitiveType = PrimitiveType::ptCone;
            res.operation     = operation;
            res.blending      = glm::clamp(blending, 0.0001f, 1.0f);
            res.color         = glm::vec4(color, 1);
            res.transform     = transform;
            res.primitiveData = { radiusBottom, radiusTop, height, rounding };
            return res;
        }
    };
    
    namespace RoundCone {
        inline glm::vec3 dimensions(const GeometryEdit& edit)
        {
            return glm::vec3(
                2.0f * glm::max(edit.primitiveData.x, edit.primitiveData.y),
                edit.primitiveData.z,
                2.0f * glm::max(edit.primitiveData.x, edit.primitiveData.y)
            ) + (edit.blending * 0.5f);
        }
        
        inline GeometryEdit createEdit(
            glm::u8   operation    = GeometryOperation::opAdd,
            Transform transform    = {},
            float32   radiusTop    = 0.125,
            float32   radiusBottom = 0.25,
            float32   height       = 1.0,
            glm::vec3 color        = rb::colors::white,
            glm::f32  blending     = 0.0f
        ) {
            GeometryEdit res;
            res.primitiveType = PrimitiveType::ptRoundCone;
            res.operation     = operation;
            res.blending      = glm::clamp(blending, 0.0001f, 1.0f);
            res.color         = glm::vec4(color, 1);
            res.transform     = transform;
            res.primitiveData = { radiusBottom, radiusTop, height, 0 };
            return res;
        }
    };
}

// common "polymorphic" operations over primitives
namespace primitives {
    
    inline glm::vec3 dimensions(const GeometryEdit& edit)
    {
        switch (edit.primitiveType)
        {
            case PrimitiveType::ptSphere:    return Sphere::dimensions(edit);
            case PrimitiveType::ptCapsule:   return Capsule::dimensions(edit);
            case PrimitiveType::ptTorus:     return Torus::dimensions(edit);
            case PrimitiveType::ptBox:       return Box::dimensions(edit);
            case PrimitiveType::ptCylinder:  return Cylinder::dimensions(edit);
            case PrimitiveType::ptCone:      return Cone::dimensions(edit);
            case PrimitiveType::ptRoundCone: return RoundCone::dimensions(edit);
            default: return glm::vec3(0);
        }
    }
     
    inline AABB computeAABB(const GeometryEdit& edit) {
        AABB bbRes = {};
        glm::vec3 halfDimensions = dimensions(edit) * 0.5f;
        bbRes.max = {  halfDimensions.x,  halfDimensions.y,  halfDimensions.z };
        bbRes.min = { -halfDimensions.x, -halfDimensions.y, -halfDimensions.z };
        return bbRes.transform(edit.transform);
    }
}

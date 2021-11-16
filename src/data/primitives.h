
#pragma once

#include <data/geometry.h>

// specific operation for particular primitive types
namespace primitives {
    namespace Sphere {
        inline glm::vec3 center(const GeometryEdit& edit)     { return edit.transform.position; }
        inline glm::f32  radius(const GeometryEdit& edit)     { return edit.primitiveData.x; }
        inline glm::vec3 dimensions(const GeometryEdit& edit) { return 2.0f * glm::vec3(radius(edit)) + (edit.blending) * 0.5f; }
        
        inline GeometryEdit createEdit(
            glm::u8   operation = GeometryOperation::opAdd,
            Transform transform = {},
            glm::f32  radius    = 1.0f,
            glm::vec3 color     = rb::colors::white,
            glm::f32  blending  = 0.0f
        ) {
            GeometryEdit res;
            res.primitiveType = PrimitiveType::ptSphere;
            res.operation     = operation;
            res.blending      = glm::u8(glm::clamp(blending * 255.0f, 0.0f, 255.0f));
            res.color         = glm::vec4(color, 1);
            res.transform     = transform;
            res.primitiveData = { radius, 0, 0, 0 };
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
            case PrimitiveType::ptSphere: return Sphere::dimensions(edit);
            default: return glm::vec3(0);
        }
    }
     
    inline AABB computeAABB(const GeometryEdit& edit) {
        AABB bbRes = {};
        glm::vec3 halfDimensions = dimensions(edit) / 2.0f;
        bbRes.max = {  halfDimensions.x,  halfDimensions.y,  halfDimensions.z };
        bbRes.min = { -halfDimensions.x, -halfDimensions.y, -halfDimensions.z };
        return bbRes.transform(edit.transform);
    }
}
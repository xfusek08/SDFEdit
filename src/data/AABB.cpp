
#include <data/AABB.h>

AABB AABB::transform(const Transform& tranform) const {
    
    // rotation requres special care to keep BB axes aligned
    if (tranform.orientation != glm::quat{0,0,0,0}) {
        glm::vec4 corners[] = {
            // right side
            { max.x, max.y, max.z, 1.0f },
            { max.x, max.y, min.z, 1.0f },
            { max.x, min.y, max.z, 1.0f },
            { max.x, min.y, min.z, 1.0f },
            
            // left side
            { min.x, max.y, max.z, 1.0f },
            { min.x, max.y, min.z, 1.0f },
            { min.x, min.y, max.z, 1.0f },
            { min.x, min.y, min.z, 1.0f },
        };
        
        auto transformMatrix = glm::inverse(tranform.getTransform());
        
        AABB bbRes = {};
        for (int i =0; i < 8; ++i) {
            auto transformed = transformMatrix * corners[i];
            bbRes.max = glm::max(bbRes.max, glm::vec3(transformed));
            bbRes.min = glm::min(bbRes.min, glm::vec3(transformed));
        }
        return bbRes;
    }
    
    // simple translation is easy
    if (tranform.position != glm::vec3(0)) {
        return AABB{
            min + tranform.position,
            max + tranform.position
        };
    }
    
    // no transform means no work
    return *this;
}

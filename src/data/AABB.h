
#pragma once

#include <data/Transform.h>

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>

struct AABB {
    glm::vec3 min = glm::vec3(FLT_MAX);
    glm::vec3 max = glm::vec3(-FLT_MAX);

    AABB() {}
    AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}

    // calculated preperties
    inline glm::vec3 center() const                     { return min + (max-min) / 2.0f; }
    inline glm::f32 distance(const AABB& otherBb) const { return glm::distance(center(), otherBb.center()); }
    inline glm::f32 longestEdgeSize() const             { return glm::compMax(max - min); }
    
    // operations
    inline static AABB add(const AABB& a, const AABB& b) {
        return { glm::min(a.min, b.min), glm::max(a.max, b.max) };
    }

    AABB transform(const Transform& transform)  const;
};

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
    glm::vec3 position    = glm::vec3({0, 0, 0});
    glm::quat orientation = glm::quat({0, 0, 0, 0});
    
    glm::mat4 getTransform() const {
        return glm::mat4_cast(orientation) * glm::translate(glm::mat4(1), position);
    }
};

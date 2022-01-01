#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
    glm::vec3 position    = glm::vec3({0, 0, 0});
    glm::quat orientation = glm::quat({0, 0, 0});
    
    Transform() {}
    Transform(glm::vec3 position, glm::vec3 rotation = {0, 0, 0}) : position(position), orientation(rotation) {}
    
    inline glm::mat4 getTransform() const {
        return glm::translate(glm::mat4(1), position) * glm::mat4_cast(orientation);
    }
    
    inline void setRotation(glm::vec3 eulerRotation) { orientation = glm::quat(eulerRotation); }
};

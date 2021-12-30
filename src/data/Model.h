
#pragma once

#include <data/Transform.h>
#include <data/geometry.h>

#include <RenderBase/defines.h>

struct Material {
    glm::vec3 color = {0, 0, 0};
    float32 shininess = 10;
};

struct Model {
    std::shared_ptr<Geometry> geometry = nullptr;
    Transform transform = {};
    Material material = {};
    
    Model(std::shared_ptr<Geometry> geometry, Transform transform = Transform()) :
        geometry(geometry),
        transform(transform)
    {}
};

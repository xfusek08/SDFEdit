
#pragma once

#include <data/Transform.h>
#include <data/geometry.h>

#include <RenderBase/defines.h>

struct Model {
    
    std::shared_ptr<Geometry> geometry;
    Transform transform;
    bool dirty;
    
    Model(std::shared_ptr<Geometry> geometry, Transform transform = Transform()) :
        geometry(geometry),
        transform(transform)
    {}
};

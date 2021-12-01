
#pragma once

#include <data/Transform.h>
#include <data/geometry.h>

#include <RenderBase/defines.h>
#include <RenderBase/ds/Pool.h>

struct Model {
    
    Model(GeometryPool::ID geometryId, Transform transform = Transform()) :
        geometryId(geometryId), transform(transform) { }
    
    Transform        transform;
    GeometryPool::ID geometryId;
    
    // TODO: additional model properties which might be used in shading
};

using ModelPool = rb::ds::Pool<Model>;

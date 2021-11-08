
#pragma once

#include <types/transform.h>

#include <RenderBase/defines.h>

struct Model {
    glm::u32  geometryId;
    Transform transform;
    // TODO: additional model properties which might be used in shading
};

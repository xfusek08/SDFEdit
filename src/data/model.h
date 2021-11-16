
#pragma once

#include <data/Transform.h>
#include <data/ItemPoolBehavior.h>

#include <RenderBase/defines.h>

struct Model {
    glm::u32  geometryId;
    Transform transform;
    // TODO: additional model properties which might be used in shading
};

// TODO: + models will be tightly packed into continuous buffer and managend by this poll, which will track changes of the models and updates them to the gpu
struct ModelPool : public ItemPoolBehavior<Model>
{
};

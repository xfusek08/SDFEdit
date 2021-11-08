#pragma once

#include <types/model.h>

#include <types/ItemPoolBehavior.h>

// TODO: + models will be tightly packed into continuous buffer and managend by this poll, which will track changes of the models and updates them to the gpu
struct ModelPool : public ItemPoolBehavior<Model>
{
};

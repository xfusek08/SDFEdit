
#pragma once

#include <types/geometry.h>

#include <types/ItemPoolBehavior.h>

#include <glad/glad.h>

// TODO: maybe avoid std::vector all togeater as future optimization and hanfle the memory myself inside pre-alllocated fixed chunks
// to me re-evaluation faster without anny mallocs
struct GeometryPool : public ItemPoolBehavior<Geometry>
{
};

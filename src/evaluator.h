
#pragma once

#include <types/volume.h>
#include <types/scene.h>

namespace evaluator {
    std::unique_ptr<Volume> buildVolumeForGeometry(const Geometry& geometry);
}

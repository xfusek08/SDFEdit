#pragma once

#include <data/model.h>
#include <data/geometry.h>
#include <data/BrickPool.h>

#include <RenderBase/defines.h>
#include <RenderBase/tools/camera.h>

#include <memory>

struct Scene
{
    std::unique_ptr<rb::CameraController> cameraController;
    std::vector<Model> models;
    bool dirty;
    uint32 division = 0;
};

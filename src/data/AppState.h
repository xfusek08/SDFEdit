#pragma once

#include <data/geometry.h>
#include <data/model.h>

#include <RenderBase/tools/camera.h>

#include <memory>

struct AppState {
    AppState() :
        modelPool(std::make_unique<ModelPool>()),
        geometryPool(std::make_unique<GeometryPool>()) {}
    std::unique_ptr<rb::CameraController> cameraController;
    std::unique_ptr<ModelPool>            modelPool;
    std::unique_ptr<GeometryPool>         geometryPool;
};

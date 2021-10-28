#pragma once

#include <components/SceneRenderer.h>
#include <components/GeometryEvaluator.h>
#include <components/GeometryPool.h>

#include <memory>

struct AppState {

    // do-er components
    SceneRenderer sceneRenderer;
    GeometryEvaluator geometryEvaluator;

    // scene data
    ModelPool modelPool;
    GeometryPool geometryPool;
    
    // user attached polymorphic types
    std::unique_ptr<rb::CameraController> cameraController;
    
    // tmp stuff
    bool isDirty = true;
    std::unique_ptr<Volume> volume;
    GLuint volumeTextureId;
};

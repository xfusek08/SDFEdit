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
    
    ModelPool    modelPool;
    GeometryPool geometryPool;
    
    // helper functions
    inline std::shared_ptr<Geometry> getModelGeometry(ModelPool::ID modelId) { return geometryPool[modelPool[modelId]->geometryId]; }
};

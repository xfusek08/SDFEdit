 #pragma once

#include <data/Model.h>
#include <data/geometry.h>
#include <data/BrickPool.h>

#include <RenderBase/defines.h>
#include <RenderBase/tools/camera.h>

#include <Vars/Vars.h>

#include <memory>

struct Scene
{
    std::unique_ptr<rb::CameraController> cameraController;
    std::vector<Model> models;
    bool dirty;
    
    // debugging data
    uint32 division = 0;
    
    vars::Vars vars; // NOTE temporary for debugging purposes until custom gui with ECS is implementred
};

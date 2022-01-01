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
    Scene() : vars(std::make_unique<vars::Vars>()) {}
    
    std::unique_ptr<rb::CameraController> cameraController;
    std::vector<Model> models;
    
    // debugging data
    std::unique_ptr<vars::Vars> vars; // NOTE temporary for debugging purposes until custom gui with ECS is implementred
};

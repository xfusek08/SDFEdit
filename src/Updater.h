
#pragma once

#include <data/Scene.h>
#include <systems/System.h>
#include <RenderBase/input.h>
#include <RenderBase/timing.h>

class Updater
{
    public:
        Updater(SystemArray systems) :
            systems(systems) {}
            
        void init(std::shared_ptr<Scene> scene);
        void onInputChange(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick);
        void onTick(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick);
        
    private:
        SystemArray systems;
};

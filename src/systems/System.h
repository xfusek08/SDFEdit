
#pragma once

#include <data/Scene.h>

class System
{
    public:
        virtual void init(std::shared_ptr<Scene> scene) {}
        virtual void onInputChange(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) {}
        virtual void onTick(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) {}
};

using SystemArray = std::vector<std::shared_ptr<System>>;

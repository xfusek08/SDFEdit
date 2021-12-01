
#pragma once

#include <data/Scene.h>

class System
{
    public:
        virtual void onInputChange(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) = 0;
        virtual void onTick(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) = 0;
};

using SystemArray = std::vector<std::shared_ptr<System>>;

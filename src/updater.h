
#pragma once

#include <types/AppState.h>
#include <RenderBase/input.h>
#include <RenderBase/timing.h>

class AppStateUpdater
{
    public:
        std::unique_ptr<AppState> onInputChanged(std::unique_ptr<AppState> oldState, const rb::input::InputState& input, const rb::timing::TimeStep& tick);
        std::unique_ptr<AppState> onTick(std::unique_ptr<AppState> oldState, const rb::input::InputState& input, const rb::timing::TimeStep& tick);
};

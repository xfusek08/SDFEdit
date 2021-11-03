
#pragma once

#include <types/AppState.h>

#include <RenderBase/gl/Program.h>

class AppStateEvaluator
{
    public:
        AppStateEvaluator();
        std::unique_ptr<AppState> evaluateState(std::unique_ptr<AppState> oldState);
        EvaluatedGeometry evaluateGeometry(const Geometry& geometry);
    private:
        rb::gl::Program program;
};


#pragma once

#include <types/AppState.h>
#include <types/EvaluatedGeometry.h>

#include <RenderBase/gl/Program.h>

class AppStateEvaluator
{
    public:
        AppStateEvaluator();
        std::unique_ptr<AppState> evaluateState(std::unique_ptr<AppState> oldState);
        std::shared_ptr<EvaluatedGeometry> evaluateGeometry(const Geometry& geometry, std::shared_ptr<EvaluatedGeometry> oldEvaluatedGeometry);
    private:
        rb::gl::Program program;
};

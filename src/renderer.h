
#pragma once

#include <types/AppState.h>

#include <RenderBase/gl/Program.h>

class AppStateRenderer
{
    public:
        AppStateRenderer();
        
        void prepare();
        void renderState(const AppState& appState);
        
    private:
        rb::gl::Program program;
};

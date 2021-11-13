
#pragma once

#include <types/AppState.h>

class VisualizationTechnique
{
    public:
        virtual void prepare(const AppState& appState) = 0;
        virtual void render(const AppState& appState) = 0;
};

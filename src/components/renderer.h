
#pragma once

#include <data/AppState.h>
#include <visualization/VisualizationTechnique.h>

#include <RenderBase/gl/Program.h>

#include <vector>
#include <memory>

/**
 * This is container and dispatcher for running for various visualization techniques.
 */
class Renderer
{
    public:
        
        using VTArray = std::vector<std::shared_ptr<VisualizationTechnique>>;
        
        Renderer(VTArray vts);
        
        void render(const AppState& appState);
        
    private:
        VTArray vts;
};

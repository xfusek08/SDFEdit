
#pragma once

#include <data/Scene.h>
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
        
        Renderer(VTArray vts);
        
        void init(std::shared_ptr<Scene> scene);
        void prepare(Scene& scene);
        void render(Scene& scene);
        
    private:
        VTArray vts;
};

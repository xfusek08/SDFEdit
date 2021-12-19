
#pragma once

#include <visualization/VisualizationTechnique.h>
#include <RenderBase/window.h>
#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>
#include <RenderBase/gl/VertexArray.h>

// this VT draws axis and it is static so it does not need scene at all
// In the future with proper ECS and rendering pipeline it will be axis entity which will use colored lines VT?

class Gui
{
    public:
        Gui(const rb::window::Window& window);
        ~Gui();
        
        void init(std::shared_ptr<Scene> scene);
        void prepare(std::shared_ptr<Scene> scene);
        void render(const Scene& scene);
    private:
        float a = 0.0;
        float b = 0.0;
        float c = 0.0;
        float d = 1.0;
};

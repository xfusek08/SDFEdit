
#pragma once

#include <visualization/VisualizationTechnique.h>
#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>
#include <RenderBase/gl/VertexArray.h>

// this VT draws axis and it is static so it does not need scene at all
// In the future with proper ECS and rendering pipeline it will be axis entity which will use colored lines VT?

class AxisVT : public VisualizationTechnique
{
    public:
        AxisVT();
        
        void prepare(Scene& scene) override;
        void render(Scene& scene) override;

    private:
        rb::gl::Program program;
        
        std::unique_ptr<rb::gl::Buffer> vertexBuffer;
        std::unique_ptr<rb::gl::Buffer> indexBuffer;
        std::unique_ptr<rb::gl::VertexArray> vertexArray;
};

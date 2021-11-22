
#pragma once

#include <visualization/VisualizationTechnique.h>
#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>
#include <RenderBase/gl/VertexArray.h>

class AxisVT : public VisualizationTechnique
{
    public:
        AxisVT();
        
        void prepare(const AppState& appState) override;
        void render(const AppState& appState) override;

    private:
        rb::gl::Program program;
        
        std::unique_ptr<rb::gl::Buffer> vertexBuffer;
        std::unique_ptr<rb::gl::Buffer> indexBuffer;
        std::unique_ptr<rb::gl::VertexArray> vertexArray;
};

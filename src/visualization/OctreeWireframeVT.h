
#pragma once

#include <visualization/VisualizationTechnique.h>

#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>
#include <RenderBase/gl/VertexArray.h>
#include <RenderBase/gl/Texture3D.h>

class OctreeWireframeVT : public VisualizationTechnique
{
    public:
        
        OctreeWireframeVT();
        void prepare(Scene& scene) override;
        void render(Scene& scene) override;
        
    private:
        rb::gl::Program octreeWireFrameProgram;
        std::unique_ptr<rb::gl::VertexArray> vertexArray;
};

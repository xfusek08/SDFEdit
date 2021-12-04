
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
        void prepare(const Scene& scene) override;
        void render(const Scene& scene) override;
        
    private:
        rb::gl::Program octreeWireFrameProgram;
        std::unique_ptr<rb::gl::VertexArray> vertexArray;
};

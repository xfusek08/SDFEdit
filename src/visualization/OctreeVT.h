
#pragma once

#include <visualization/VisualizationTechnique.h>

#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>
#include <RenderBase/gl/VertexArray.h>

/**
 * Octree visualization technique
 *
 * This class draws wireframe representation of octree of given set of models from current scene state.
 */
class OctreeVT : public VisualizationTechnique
{
    public:
    
        OctreeVT();
        void prepare(const AppState& appState) override;
        void render(const AppState& appState) override;

    private:
        rb::gl::Program program;
    
        std::unique_ptr<rb::gl::Buffer> vertexBuffer;
        std::unique_ptr<rb::gl::VertexArray> vertexArray;
};

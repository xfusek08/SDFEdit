
#pragma once

#include <visualization/VisualizationTechnique.h>

#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>
#include <RenderBase/gl/VertexArray.h>
#include <RenderBase/gl/Texture3D.h>

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
        rb::gl::Program octreeWireFrameProgram;
        rb::gl::Program brickRenderProgram;
        
        uint32 nodeCount = 0;
        uint32 brickCount = 0;
        rb::gl::Program octreeEvaluationProgram;
        rb::gl::Program octreeInitiationProgram;
        
        std::unique_ptr<rb::gl::Buffer> vertexBuffer;
        std::unique_ptr<rb::gl::Buffer> brickHelperBuffer;
        std::unique_ptr<rb::gl::VertexArray> vertexArray;
        
        std::unique_ptr<rb::gl::Buffer> nodeBuffer;
        std::unique_ptr<rb::gl::Buffer> counterBuffer;
        
        std::unique_ptr<rb::gl::Buffer> editBuffer;
        std::unique_ptr<rb::gl::Texture3D>  volumeTexture;
        
        uint currentLevelBeginIndex;
        uint nodesInCurrentLevel;
        
        bool prepared = false;
};

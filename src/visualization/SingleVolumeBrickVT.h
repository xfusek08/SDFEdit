
#pragma once

#include <visualization/VisualizationTechnique.h>

#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>
#include <RenderBase/gl/Texture3D.h>

class SingleVolumeBrickVT : public VisualizationTechnique
{
    public:
    
        SingleVolumeBrickVT();
        
        void prepare(const AppState& appState) override;
        void render(const AppState& appState) override;
        
    private:
        rb::gl::Program renderProgram;
        rb::gl::Program computeProgram;
        
        GLuint vao;
        std::unique_ptr<rb::gl::Buffer> editsBuffer = nullptr;
        std::unique_ptr<rb::gl::Texture3D> volumeTexture = nullptr;
        uint32  voxelCount = 0;
        float32 voxelSize  = 0;
};

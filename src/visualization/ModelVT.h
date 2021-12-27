#pragma once

#include <visualization/VisualizationTechnique.h>

#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>
#include <RenderBase/gl/VertexArray.h>

#include <unordered_map>

/**
 * This class will take care of rendering all models which has link to a geometry
 */
class ModelVT : public VisualizationTechnique
{
    public:
    
        ModelVT();
        /**
         * This function will evaluate all changed geometries and
         */
        void prepare(const Scene& scene) override;
        void render(const Scene& scene) override;
        
    private:
    
        struct RenderBatchSOA {
            std::unique_ptr<rb::gl::VertexArray> vao = nullptr;
            std::vector<glm::uvec2> toRenderNodes = {};
            std::unique_ptr<rb::gl::Buffer> toRenderNodesBuffer = nullptr;
        };
        
        rb::gl::Program renderProgram;
        rb::gl::Program brickShellProgram;
        std::unordered_map<Geometry*, RenderBatchSOA> geometryBatches;

        // model transforms are global for all geometries
        std::vector<glm::mat4>  transforms = {};
        std::unique_ptr<rb::gl::Buffer> transformBuffer = nullptr;
};

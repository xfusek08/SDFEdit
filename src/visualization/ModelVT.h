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
            std::vector<glm::vec4> translations = {};
            std::vector<glm::uvec2> toRenderNodes = {};
            
            std::unique_ptr<rb::gl::VertexArray> vao = nullptr;
            std::unique_ptr<rb::gl::Buffer> translationBuffer = nullptr;
            std::unique_ptr<rb::gl::Buffer> toRenderNodesBuffer = nullptr;
        };
        
        rb::gl::Program renderProgram;
        std::unordered_map<Geometry*, RenderBatchSOA> geometryBatches;
};

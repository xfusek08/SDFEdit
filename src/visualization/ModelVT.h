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
        
        void init(std::shared_ptr<Scene> scene) override;
        void prepare(Scene& scene) override;
        void render(Scene& scene) override;
        
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
        
        // temporary color buffer
        std::vector<Material>  materials = {};
        std::unique_ptr<rb::gl::Buffer> materialsBuffer = nullptr;
};

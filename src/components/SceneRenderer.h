#pragma once

#include <components/ModelPool.h>
#include <components/GeometryPool.h>

#include <RenderBase/gl/Program.h>
#include <RenderBase/tools/camera.h>

struct SceneRenderer {
    
    SceneRenderer();
    
    /** Prepares GPU to be ready for the rendering */
    void prepare(const ModelPool& modelPool, const GeometryPool& geometrypool, rb::Camera& camera);
    
    /** Renders frame */
    void render();
    
    private:
        rb::gl::Program program;
        
        //NOTE: temp implementation
        std::vector<EvaluatedGeometry> toRenderGeometry;
};

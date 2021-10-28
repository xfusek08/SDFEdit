#pragma once

#include<types/scene.h>
#include<types/volume.h>

#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/SparseTexture3D.h>

struct EvaluatedGeometry {
    // TODO: new buffer with node tree
    
    // old tmp implementation
    std::unique_ptr<Volume> volume;
};

struct GeometryEvaluator {
    
    GeometryEvaluator();
    
    /**
     * This function evaluates the geometry and fill given brick pool with its bricks
     */
    EvaluatedGeometry evaluate(const Geometry& geometry, rb::gl::SparseTexture3D& brickPool) const;
    
    private:
        rb::gl::Program program;
};


#include <components/GeometryEvaluator.h>

#include <evaluator.h>

GeometryEvaluator::GeometryEvaluator() :
    program(
        // TODO: insert compute shader here
    )
{
    
}

EvaluatedGeometry GeometryEvaluator::evaluate(const Geometry& geometry, rb::gl::SparseTexture3D& brickPool) const
{
    // TODO: dispatch cs pipeline to evaluate geometry
    
    // old tmp implementation
    return { evaluator::buildVolumeForGeometry(geometry) };
}
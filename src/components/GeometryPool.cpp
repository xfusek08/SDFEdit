
#include <components/GeometryPool.h>

using namespace std;

GeometryPool::~GeometryPool()
{
    // TODO: free up all allocated gpu memory
    
    // NOTE: temp implementation just to get compute shader working
    for (const auto& geom : evaluatedGeometries) {
        glDeleteTextures(1, &geom.volumeTextrue);
    }
}

void GeometryPool::evaluateGeometries(GeometryEvaluator& evaluator)
{
    // TODO: use evaluator evaluate all invalid geometries and micro-manage their states on gpu.

    // NOTE: temp implementation with single flat 3d texture per geometry
    rb::gl::SparseTexture3D dummyTexture;
    evaluatedGeometries.resize(items.size());
    for (int i = 0; i < items.size(); ++i) {
        evaluatedGeometries[i] = evaluator.evaluate(items[i], dummyTexture);
    }
}

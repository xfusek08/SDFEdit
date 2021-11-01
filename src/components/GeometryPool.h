#pragma once

#include <types/scene.h>

#include <components/ElementPoolBehavior.h>
#include <components/GeometryEvaluator.h>

// TODO: maybe avoid std::vector all togeater as future optimization and hanfle the memory myself inside pre-alllocated fixed chunks
// to me re-evaluation faster without anny mallocs
struct GeometryPool : public ItemPoolBehavior<Geometry> {
    // TODO: manipulation with individual geometries will be provided by interface to ensure keeping track of
    
    GeometryPool() {}
    
    ~GeometryPool();

    /**
     * Makes sure that changes in geometries will be loaded and evaluated to the gpu using given evaluator.
     */
    void evaluateGeometries(GeometryEvaluator& evaluator);
    
    inline const std::vector<EvaluatedGeometry>& getEvaluatedGeometries() const { return  evaluatedGeometries; }
    
    private:
        // TODO: + array of buffers one for each geometry node tree
        // TODO: buffer (sparse 3D texture) of brick bool shared by all the geometries
        
        // NOTE: temp implementation with single flat 3d texture
        std::vector<EvaluatedGeometry> evaluatedGeometries;
};

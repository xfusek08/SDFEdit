#pragma once

#include <types/scene.h>

#include <components/ElementPoolBehavior.h>
#include <components/GeometryEvaluator.h>

struct GeometryPool : public ItemPoolBehavior<Geometry> {
    // TODO: manipulation with individual geometries will be provided by interface to ensure keeping track of
    
    GeometryPool() {}
    
    ~GeometryPool();
    
    /**
     * Makes sure that changes in geometries will be loaded and evaluated to the gpu using given evaluator.
     */
    void evaluateGeometries(const GeometryEvaluator& evaluator);
    
    
    // old tmp implementation
    const Volume& getVolume() const { return *volume; }
    GLuint volumeTextureId() const { return texId; }
    
    private:
        // TODO: + array of buffers one for each geometry node tree
        // TODO: buffer (sparse 3D texture) of brick bool shared by all the geometries
        
        // tmp old implementation
        GLuint texId = -1;
        std::unique_ptr<Volume> volume;
};

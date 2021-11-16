#pragma once

#include <data/geometry.h>

#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>

#include <memory>

struct EvaluatedGeometry
{
    std::unique_ptr<rb::gl::Buffer> nodeBuffer;
};

// This class manages pool of evaluated geometries on the gpu
class GeometryEvaluator
{
    public:
        GeometryEvaluator();
        ~GeometryEvaluator();

        void updateGeometries(const GeometryPool& geometryPool);
        
        inline std::shared_ptr<EvaluatedGeometry> getEvaluatedGeometry(uint32 geometryId) const { return evaluatedGeometries[geometryId]; }
        
    private:
        // TODO: add evaluation program
        std::vector<std::shared_ptr<EvaluatedGeometry>> evaluatedGeometries;
};

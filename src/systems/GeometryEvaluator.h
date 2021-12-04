#pragma once

#include <systems/System.h>

#include <data/geometry.h>

#include <RenderBase/gl/Program.h>
#include <RenderBase/gl/Buffer.h>

#include <memory>

// This class manages pool of evaluated geometries on the gpu
class GeometryEvaluator : public System
{
    public:
        GeometryEvaluator();
        
        void init(std::shared_ptr<Scene> scene) override;
        void onInputChange(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) override;
        void onTick(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) override;
        
    private:
        // evaluator settings
        uint32 maxSubdivisions = 4;
        
        // evaluated internal state
        rb::gl::Program octreeEvaluationProgram;
        rb::gl::Program octreeInitiationProgram;
        std::unique_ptr<rb::gl::Buffer> editBuffer = nullptr;
        std::unique_ptr<rb::gl::Buffer> counterBuffer = nullptr;
        
        // internal methods
        void evaluateScene(std::shared_ptr<Scene> scene) const;
        std::shared_ptr<SVOctree> evaluateGeometry(const Geometry& geometry) const;
        void loadEditBuffer(const Geometry& geometry) const;
};

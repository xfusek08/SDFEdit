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

        void onInputChange(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) override;
        void onTick(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick) override;
        
    private:
        void evaluateScene(std::shared_ptr<Scene> scene);
        void evaluateGeometry(std::shared_ptr<Geometry> geometry);
};

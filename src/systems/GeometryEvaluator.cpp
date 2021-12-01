
#include <systems/GeometryEvaluator.h>

GeometryEvaluator::GeometryEvaluator()
{
    
}

void GeometryEvaluator::onInputChange(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick)
{
    evaluateScene(scene);
}

void GeometryEvaluator::onTick(std::shared_ptr<Scene> scene, const rb::input::InputState& input, const rb::timing::TimeStep& tick)
{
    // geometry propably won't be animated
    // uncomment of geometry could't change in time update cycle
    evaluateScene(scene);
}

void GeometryEvaluator::evaluateScene(std::shared_ptr<Scene> scene)
{
    for (auto geometry : scene->geometryPool) {
        if (geometry != nullptr && geometry->dirty) {
            evaluateGeometry(geometry);
        }
    }
}

void GeometryEvaluator::evaluateGeometry(std::shared_ptr<Geometry> geometry)
{
    
}

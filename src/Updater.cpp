
#include <Updater.h>

#include <data/primitives.h>

#include <RenderBase/logging.h>

#include <RenderBase/tools/random.h>

using namespace std;
using namespace rb;

void Updater::init(shared_ptr<Scene> scene)
{
    for (auto& system : systems) {
        system->init(scene);
    }
}

void Updater::onInputChange(shared_ptr<Scene> scene, const input::InputState& input, const timing::TimeStep& tick)
{
    // TODO: for each updatable item ins scene:
    
    // TODO: camera system? because camera constroller is system of its own technically
    // just stored inside scene instead of Updater
    // ...
    // Maybe just has array ov VTs and array of Systems stored inside Scene ass well and iterate in draw and update method directly ...
    scene->cameraController->onInputChange(input, tick);
    
    for (auto& system : systems) {
        system->onInputChange(scene, input, tick);
    }
}

void Updater::onTick(shared_ptr<Scene> scene, const input::InputState& input, const timing::TimeStep& tick)
{
    // TODO: for each updatable item ins scene:
    scene->cameraController->onTick(input, tick);
    for (auto& system : systems) {
        system->onTick(scene, input, tick);
    }
    
    if (tick.order % 60 == 0) {
        scene->division++;
    }
}

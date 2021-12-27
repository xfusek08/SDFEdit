
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
    
    if (input.isKeyPressed(GLFW_KEY_SPACE)) {
        scene->division++;
    }
}

glm::vec3 dir = glm::vec3(0, 0.1, 0);
void Updater::onTick(shared_ptr<Scene> scene, const input::InputState& input, const timing::TimeStep& tick)
{
    // TODO: for each updatable item ins scene:
    scene->cameraController->onTick(input, tick);
    for (auto& system : systems) {
        system->onTick(scene, input, tick);
    }
    
    // if (scene->models.size() > 1) {
    //     scene->models[2].transform.position = scene->models[2].transform.position + dir;
    //     scene->models[2].transform.orientation = glm::normalize(scene->models[2].transform.orientation * glm::quat(glm::vec3{0.0, 0.01, 0.0}));
    //     scene->models[1].transform.orientation = glm::normalize(scene->models[1].transform.orientation * glm::quat(glm::vec3{0.02, 0.02, 0.02}));
    // }
    
    // if (scene->models[2].transform.position.y > 2 || scene->models[2].transform.position.y < 0) {
    //     dir.y *= -1;
    // }
    
    // if (tick.order % 60 == 0) {
    //     scene->division++;
    // }
}


#include <components/updater.h>

#include <data/primitives.h>

#include <RenderBase/tools/random.h>

using namespace std;
using namespace rb;

unique_ptr<AppState> AppStateUpdater::onInputChanged(unique_ptr<AppState> oldState, const input::InputState& input, const timing::TimeStep& tick)
{
    oldState->cameraController->onInputChange(input, tick);
    
    // Generate new edit list when space is pressed
    if (input.isKeyPressed(GLFW_KEY_SPACE)) {
        Geometry& geometry = oldState->geometryPool->getItem(0);
        geometry.clearEdits();
        glm::vec3 min = glm::vec3{-2, -2, -2};
        glm::vec3 max = glm::vec3{2, 2, 2};
        for (int i = 0; i < 100; ++i) {
            geometry.addEdit(
                primitives::Sphere::createEdit(opAdd, Transform(randomPosition(min, max)), randomFloat(0.001, 0.7))
            );
        }
    }
    
    return move(oldState);
}

unique_ptr<AppState> AppStateUpdater::onTick(unique_ptr<AppState> oldState, const input::InputState& input, const timing::TimeStep& tick)
{
    oldState->cameraController->onTick(input, tick);
    return move(oldState);
}

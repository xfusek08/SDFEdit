
#include <components/AppState.h>
#include <types/primitives.h>

#include <RenderBase/rbapp.h>
#include <RenderBase/tools/random.h>

#include <evaluator.h>

using namespace std;
using namespace rb;

unique_ptr<AppState> appState;

bool Application::init()
{
    appState = make_unique<AppState>();
    
    // set up main camera
    Camera cam = Camera({0, 0, -10});
    cam.setAspectRatio(float(state.window->getWidth()) / float(state.window->getHeight()));
    appState->cameraController = make_unique<OrbitCameraController>(cam, *state.eventDispatcher);
    
    // prepare test geometry
    appState->geometryPool.addItems({ Geometry(70) });
    appState->modelPool.addItems({ { 0, Transform() } });
    glm::vec3 min = glm::vec3{-2, -2, -2};
    glm::vec3 max = glm::vec3{2, 2, 2};
    for (int i = 0; i < 10; ++i) {
        appState->geometryPool.getItem(0).addEdit(primitives::Sphere::createEdit(opAdd, Transform(randomPosition(min, max)), randomFloat(0.001, 0.7)));
    }
    
    appState->geometryPool.evaluateGeometries(appState->geometryEvaluator);
    
    // Main Event reactions
    
    // Exit on escape
    subscribeToEvent(events::EVENT_CODE_KEY_PRESSED, [this](events::Event event) {
        if (event.data.u16[0] == GLFW_KEY_ESCAPE) {
            state.status = app::Status::Exited;
            return true;
        }
        return false;
    });
    
    subscribeToEvent(EVENT_CODE_CAMERA_CHANGED, [this](events::Event event) {
        appState->isDirty = true;
        return false; // let other system know this event
    });
    
    return true;
}

void Application::draw()
{
    if (appState->isDirty) {
        appState->sceneRenderer.prepare(appState->modelPool, appState->geometryPool, appState->cameraController->getCamera());
        appState->isDirty = false;
    }
    appState->sceneRenderer.render();
}

bool Application::finalize()
{
    
    appState.reset();
    return true;
}

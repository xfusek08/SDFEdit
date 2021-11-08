
#include <types/AppState.h>
#include <types/primitives.h>

#include <RenderBase/rbapp.h>
#include <RenderBase/tools/random.h>

#include <evaluator.h>
#include <updater.h>
#include <renderer.h>

using namespace std;
using namespace rb;

unique_ptr<AppState>          appState;
unique_ptr<AppStateUpdater>   updater;
unique_ptr<AppStateEvaluator> evaluator;
unique_ptr<AppStateRenderer>  renderer;

bool Application::init()
{
    // App general settings
    // --------------------
    
    appState  = make_unique<AppState>();
    updater   = make_unique<AppStateUpdater>();
    evaluator = make_unique<AppStateEvaluator>();
    renderer  = make_unique<AppStateRenderer>();
    
    // Camera Set up
    // -------------
    
    Camera cam = Camera({0, 0, 10});
    cam.setAspectRatio(float(window->getWidth()) / float(window->getHeight()));
    appState->cameraController = make_unique<OrbitCameraController>(cam);
    
    // Preparing the test geometry
    // ----------------------
    
    appState->geometryPool->addItems({ Geometry(100) });
    appState->modelPool->addItems({ { 0, Transform() } });
    // appState->geometryPool->getItem(0).addEdit(primitives::Sphere::createEdit());
    
    glm::vec3 min = glm::vec3{-2, -2, -2};
    glm::vec3 max = glm::vec3{2, 2, 2};
    for (int i = 0; i < 100; ++i) {
        appState->geometryPool->getItem(0).addEdit(
            primitives::Sphere::createEdit(opAdd, Transform(randomPosition(min, max)), randomFloat(0.001, 0.7))
        );
    }
    
    renderer->prepare();
    return true;
}

bool Application::onInputChange(const input::InputState& input, const timing::TimeStep& tick)
{
    if (input.isKeyPressed(GLFW_KEY_ESCAPE)) {
        exit();
        return true;
    }
    
    appState = updater->onInputChanged(move(appState), input, tick);
    return false;
}

bool Application::onTick(const input::InputState& input, const timing::TimeStep& tick)
{
    appState = updater->onTick(move(appState), input, tick);
    return false;
}

bool Application::onResize(uint32 newWidth, uint32 newHeight)
{
    appState->cameraController->getCamera().setAspectRatio(float32(newWidth) / float32(newHeight));
    return false;
}

void Application::draw()
{
    appState = evaluator->evaluateState(move(appState));
    renderer->renderState(*appState);
}


bool Application::finalize()
{
    appState.reset();
    updater.reset();
    evaluator.reset();
    renderer.reset();
    return true;
}


#include <data/AppState.h>
#include <data/primitives.h>

#include <RenderBase/tools/random.h>
#include <RenderBase/application.h>

#include <visualization/SingleVolumeBrickVT.h>
#include <visualization/OctreeVT.h>

#include <updater.h>
#include <renderer.h>

using namespace std;
using namespace rb;

class Application : public app::BasicOpenGLApplication
{
    using BasicOpenGLApplication::BasicOpenGLApplication;
    
    unique_ptr<AppState>          appState;
    unique_ptr<AppStateUpdater>   updater;
    
    unique_ptr<Renderer> renderer;
    
    bool init() override
    {
        // App general settings
        // --------------------
        
        appState = make_unique<AppState>();
        updater  = make_unique<AppStateUpdater>();
        renderer = make_unique<Renderer>(Renderer::VTArray{
            // TMP VTs
            make_shared<SingleVolumeBrickVT>(),
            make_shared<OctreeVT>(),
        });
        
        // Camera Set up
        // -------------
        
        Camera cam = Camera({0, 0, 10});
        cam.setAspectRatio(float(window->getWidth()) / float(window->getHeight()));
        appState->cameraController = make_unique<OrbitCameraController>(cam);
        
        // Preparing the test geometry
        // ----------------------
        
        appState->geometryPool->addItems({ Geometry(5) });
        appState->modelPool->addItems({ { 0, Transform() } });
        
        appState->geometryPool->getItem(0).addEdit(primitives::Sphere::createEdit());
        
        // glm::vec3 min = glm::vec3{-2, -2, -2};
        // glm::vec3 max = glm::vec3{2, 2, 2};
        // for (int i = 0; i < 100; ++i) {
        //     appState->geometryPool->getItem(0).addEdit(
        //         primitives::Sphere::createEdit(opAdd, Transform(randomPosition(min, max)), randomFloat(0.001, 0.7))
        //     );
        // }
        return true;
    }

    void draw() override
    {
        renderer->render(*appState);
        appState->cameraController->getCamera().dirtyFlag = false;
    }
    
    bool onInputChange(const input::InputState& input, const timing::TimeStep& tick) override
    {
        if (input.isKeyPressed(GLFW_KEY_ESCAPE)) {
            exit();
            return true;
        }
        
        appState = updater->onInputChanged(move(appState), input, tick);
        return false;
    }

    bool onTick(const input::InputState& input, const timing::TimeStep& tick) override
    {
        appState = updater->onTick(move(appState), input, tick);
        return false;
    }

    bool onResize(uint32 newWidth, uint32 newHeight) override
    {
        appState->cameraController->getCamera().setAspectRatio(float32(newWidth) / float32(newHeight));
        return false;
    }
};

int main(int argc, char** argv)
{
    auto config = app::loadDefaultConfigFromArguments(argc, argv);
    auto app = Application(config);
    return app.run() ? 0 : 1;
}

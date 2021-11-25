
#include <data/AppState.h>
#include <data/primitives.h>

#include <RenderBase/tools/random.h>
#include <RenderBase/application.h>

#include <visualization/SingleVolumeBrickVT.h>
#include <visualization/OctreeVT.h>
#include <visualization/AxisVT.h>

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
            // make_shared<SingleVolumeBrickVT>(),
            make_shared<OctreeVT>(),
            make_shared<AxisVT>()
        });
        
        // Camera Set up
        // -------------
        
        Camera cam = Camera({0, 0, 10});
        cam.setAspectRatio(float(window->getWidth()) / float(window->getHeight()));
        appState->cameraController = make_unique<OrbitCameraController>(cam);
        
        // Preparing the test geometry
        // ----------------------
        
        appState->geometryPool->addItems({ Geometry(8) });
        appState->modelPool->addItems({ { 0, Transform() } });
        
        // appState->geometryPool->getItem(0).addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0,0,0}), 0.5));
        // appState->geometryPool->getItem(0).addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({1,2,0}), 0.3));
        
        glm::vec3 min = glm::vec3{-10, -10, -10};
        glm::vec3 max = glm::vec3{10, 10, 10};
        for (int i = 0; i < 100; ++i) {
            appState->geometryPool->getItem(0).addEdit(
                primitives::Sphere::createEdit(opAdd, Transform(randomPosition(min, max)), randomFloat(0.01, 1.5))
            );
        }
        
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

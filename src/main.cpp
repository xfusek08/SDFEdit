
#include <data/Scene.h>
#include <data/primitives.h>

#include <RenderBase/tools/random.h>
#include <RenderBase/application.h>

#include <visualization/SingleVolumeBrickVT.h>
#include <visualization/OctreeVT.h>
#include <visualization/AxisVT.h>
#include <visualization/OctreeWireframeVT.h>

#include <systems/GeometryEvaluator.h>

#include <Updater.h>
#include <Renderer.h>

#include <Renderer.h>

using namespace std;
using namespace rb;

class Application : public app::BasicOpenGLApplication
{
    using BasicOpenGLApplication::BasicOpenGLApplication;
    
    shared_ptr<Scene>    scene;
    unique_ptr<Updater>  updater;
    unique_ptr<Renderer> renderer;
    
    bool init() override
    {
        // App general settings
        // --------------------
        
        scene = make_shared<Scene>();
        
        updater = make_unique<Updater>(SystemArray{
            make_shared<GeometryEvaluator>(),
        });
        
        renderer = make_unique<Renderer>(VTArray{
            // make_shared<SingleVolumeBrickVT>(),
            // make_shared<OctreeVT>(),
            make_shared<OctreeWireframeVT>(),
            make_shared<AxisVT>(),
        });
        
        // Camera Set up
        // -------------
        
        Camera cam = Camera({0, 0, 10});
        cam.setAspectRatio(float(window->getWidth()) / float(window->getHeight()));
        scene->cameraController = make_unique<OrbitCameraController>(cam);
        
        // Preparing the test geometry
        // ----------------------
        
        auto modelId = scene->modelPool.add(make_shared<Model>(
            scene->geometryPool.add(make_shared<Geometry>(8))
        ));
        
        auto geometry = scene->getModelGeometry(modelId);
        geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0,0,0}), 0.5));
        
        
        // glm::vec3 min = glm::vec3{-10, -10, -10};
        // glm::vec3 max = glm::vec3{10, 10, 10};
        // for (int i = 0; i < 100; ++i) {
        //     geometry->.addEdit(
        //         primitives::Sphere::createEdit(opAdd, Transform(randomPosition(min, max)), randomFloat(0.01, 1.5))
        //     );
        // }
        
        updater->init(scene);
        renderer->init(scene);
        return true;
    }
    
    bool onResize(uint32 newWidth, uint32 newHeight) override
    {
        scene->cameraController->getCamera().setAspectRatio(float32(newWidth) / float32(newHeight));
        return false;
    }

    bool onInputChange(const input::InputState& input, const timing::TimeStep& tick) override
    {
        if (input.isKeyPressed(GLFW_KEY_ESCAPE)) {
            exit();
            return true;
        }
        
        updater->onInputChange(scene, input, tick);
        return false;
    }

    bool onTick(const input::InputState& input, const timing::TimeStep& tick) override
    {
        updater->onTick(scene, input, tick);
        return false;
    }

    void draw() override
    {
        renderer->prepare(*scene);
        renderer->render(*scene);
        scene->cameraController->getCamera().dirtyFlag = false;
    }
};

int main(int argc, char** argv)
{
    auto config = app::loadDefaultConfigFromArguments(argc, argv);
    auto app = Application(config);
    return app.run() ? 0 : 1;
}

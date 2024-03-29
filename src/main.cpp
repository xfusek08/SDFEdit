
#include <data/Scene.h>
#include <data/primitives.h>

#include <imguiVars/addVarsLimits.h>

#include <RenderBase/tools/random.h>
#include <RenderBase/application.h>
#include <RenderBase/tools/utils.h>
#include <RenderBase/logging.h>

#include <glm/gtx/string_cast.hpp>

#include <visualization/AxisVT.h>
#include <visualization/OctreeWireframeVT.h>
#include <visualization/ModelVT.h>

#include <systems/GeometryEvaluator.h>
#include <systems/RandomMovementTestBench.h>

#include <Updater.h>
#include <Renderer.h>
#include <Gui.h>

#include <SceneLoader.h>

#include <Renderer.h>

using namespace std;
using namespace rb;

class Application : public app::BasicOpenGLApplication
{
    using BasicOpenGLApplication::BasicOpenGLApplication;
    
    shared_ptr<Scene>    scene;
    unique_ptr<Updater>  updater;
    unique_ptr<Renderer> renderer;
    unique_ptr<Gui>      gui;
    
    shared_ptr<RandomMovementTestBench> testBench;
    
    vector<const char*> sceneNames = {
        RESOURCE_SCENES_BASIC_SCENE_JSON,
        RESOURCE_SCENES_CHESS_BOARD_JSON,
        RESOURCE_SCENES_KNIGHT_TEST_JSON,
        RESOURCE_SCENES_MUSHED_JSON,
        "stress"
    };
    
    enum Scenes {
        a,b,c,d,stress
    };

    bool init() override
    {
        // App general settings
        // --------------------
        
        scene = prepareSharedSceneData(sceneNames[a]);
        // scene = make_shared<Scene>();
        
        // scene switching
        scene->vars->addEnum<Scenes>("scene", a);
        addEnumValues<Scenes>(*scene->vars, rb::utils::genRange<int32_t>(0, sceneNames.size(), 1), sceneNames);
        scene->vars->setChangeCallback("scene", [&]() {
            *scene->vars->addOrGet<const char*>("reloadTo", nullptr) = sceneNames[*scene->vars->get<Scenes>("scene")];
        });
        
        auto evaluator = make_shared<GeometryEvaluator>();
        testBench = make_shared<RandomMovementTestBench>(AABB({-1,-1,-1}, {1,1,1}), evaluator);
        updater = make_unique<Updater>(SystemArray{
            testBench,
            evaluator,
        });
        
        renderer = make_unique<Renderer>(VTArray{
            make_shared<ModelVT>(),
            make_shared<OctreeWireframeVT>(),
            make_shared<AxisVT>(),
        });
        
        gui = make_unique<Gui>(*window);
        
        // Camera Set up
        // -------------
        
        Camera cam = Camera({0, 0, 10});
        cam.setAspectRatio(float(window->getWidth()) / float(window->getHeight()));
        scene->cameraController = make_unique<FreeCameraController>(cam);
        
        // // Preparing the test geometry
        // // ----------------------
        
        // // auto sphereGeometry = make_shared<Geometry>(8);
        // // sphereGeometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0, 0.5, 0})));
        
        // auto boxGeometry = make_shared<Geometry>(8);
        // boxGeometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0, -0.5 ,0}), 1.5));
        // boxGeometry->addEdit(primitives::Box::createEdit(GeometryOperation::opSubtract, Transform({0, 1, 0}, {glm::quarter_pi<float32>(), glm::quarter_pi<float32>(), 0}), glm::vec3(1.0f), 0.2, rb::colors::white, 0.3 ));
        
        // scene->models.push_back(Model(boxGeometry, Transform(glm::vec3(0), {glm::quarter_pi<float32>(), 0, 0})));
        // // scene->models.push_back(Model(sphereGeometry));
        
        // // for (auto& m : scene->models) {
        // //     RB_DEBUG(glm::to_string(m.geometry->getAABB().min));
        // // }
        
        // geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0, -1, 0})));
        // geometry->addEdit(primitives::Box::createEdit(GeometryOperation::opAdd, Transform({0,2.5,0}), {0.75, 0.75, 0.75}));
        // geometry->addEdit(primitives::Box::createEdit(GeometryOperation::opAdd, Transform({0,4.5,0}), {0.75, 0.75, 0.75}));
        // geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({16,16,0}), 7));
        // geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({-2,2,0}), 1.3));
        
        // scene->models[0].geometry = geometry;
        
        // scene->models.push_back(Model(geometry, Transform({10, 0, -10})));
        // scene->models.push_back(Model(geometry, Transform({10, 0, 10})));
        // scene->models.push_back(Model(geometry, Transform({-10, 0, 10})));
        // scene->models.push_back(Model(geometry, Transform({-10, 0, -10})));
        
        // // scene->models.push_back(Model(geometry, Transform({4, 0, 0})));
        // // scene->models.push_back(Model(geometry, Transform({8, 0, 0})));
        
        // // geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0,0, 1.5}), 0.5));
        // // geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0,0, 2 * 1.5}), 0.5));
        // // geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0,0, 3 * 1.5}), 0.5));
        // // geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0,0, 4 * 1.5}), 0.5));
        // // geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({0,0, 5 * 1.5}), 0.5));
        
        
        // // glm::vec3 min = glm::vec3{-10, -10, -10};
        // // glm::vec3 max = glm::vec3{10, 10, 10};
        // // for (int i = 0; i < 100; ++i) {
        // //     geometry->addEdit(
        // //         primitives::Sphere::createEdit(opAdd, Transform(randomPosition(min, max)), randomFloat(0.05, 3))
        // //     );
        // // }
        
        gui->init(scene);
        renderer->init(scene);
        updater->init(scene);
        
        gui->prepare(scene);
                
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
        auto reloadTo = scene->vars->addOrGet<const char*>("reloadTo", nullptr);
        if (*reloadTo != nullptr) {
            testBench->boundarries = AABB(glm::vec3(-1), glm::vec3(1));
            auto newScene = scene->vars->getEnum<Scenes>("scene") == stress ? loadStressScene() : prepareSharedSceneData(*reloadTo);
            newScene->cameraController = move(scene->cameraController);
            newScene->vars = move(scene->vars);
            scene = newScene;
            gui->init(scene);
            renderer->init(scene);
            updater->init(scene);
            *reloadTo = nullptr;
        }
        
        updater->onTick(scene, input, tick);
        return false;
    }
    
    void draw() override
    {
        gui->prepare(scene);
        renderer->prepare(*scene);
        renderer->render(*scene);
        gui->render(*scene);
        scene->cameraController->getCamera().dirtyFlag = false;
    }
    
    shared_ptr<Scene> loadStressScene()
    {
        auto newScene = make_shared<Scene>();
        auto geometry = make_shared<Geometry>();
        
        for (int x = 0; x < 6; ++x) {
            for (int y = 0; y < 6; ++y) {
                for (int z = 0; z < 6; ++z) {
                    // geometry->addEdit(primitives::Cone::createEdit(GeometryOperation::opAdd, Transform({x - 3, y - 3, z - 3})));
                    geometry->addEdit(primitives::Sphere::createEdit(GeometryOperation::opAdd, Transform({x - 3, y - 3, z - 3})));
                }
            }
        }
        
        newScene->models.push_back(Model(geometry));
        testBench->boundarries = AABB(glm::vec3(-3), glm::vec3(3));
        return newScene;
    }
};

int main(int argc, char** argv)
{
    auto config = app::loadDefaultConfigFromArguments(argc, argv);
    auto app = Application(config);
    return app.run() ? 0 : 1;
}

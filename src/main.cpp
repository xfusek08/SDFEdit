#include <evaluator.h>

#include <types/primitives.h>
#include <types/scene.h>
#include <types/volume.h>

#include <RenderBase/rbapp.h>
#include <RenderBase/tools/camera.h>

#include <iostream>1
#include <iomanip>

using namespace std;
using namespace rb;

// scene state
Scene* scene;
Volume* volume;
OrbitCameraController* orbitCamera;

// gl things
gl::Program* program;
gl::Texture3D* volumeDistanceTexture;
gl::Texture3D* volumeColorTexture;
GLuint vao;

bool Application::init()
{
    // event handeling
    
    subscribeToEvent(events::EVENT_CODE_KEY_PRESSED, [&](events::Event event) {
        if (event.data.u16[0] == GLFW_KEY_ESCAPE) {
            state.status = app::Status::Exited;
            return true;
        }
        return false;
    });
    
    // create program
    
    program = new gl::Program(
        new gl::Shader(GL_VERTEX_SHADER, RESOURCE_SHADERS_SDF_BRICK_VS),
        // new gl::Shader(GL_VERTEX_SHADER, RESOURCE_SHADERS_SCREEN_QUAD_VS),
        new gl::Shader(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_SDF_BRICK_FS)
    );
    
    // Prepare camera + its control
    
    orbitCamera = new OrbitCameraController(Camera({0, 0, -10}));
    orbitCamera->getCamera().setAspectRatio(float(state.window->getWidth()) / float(state.window->getHeight()));
    program->loadStandardCamera(orbitCamera->getCamera());
    
    state.eventDispatcher->subscribeToEvent(events::EVENT_CODE_KEY_PRESSED, orbitCamera, [&](events::Event event) {
        switch (event.data.u16[0])
        {
            case GLFW_KEY_LEFT:  orbitCamera->moveLeft();  break;
            case GLFW_KEY_RIGHT: orbitCamera->moveRight(); break;
            case GLFW_KEY_UP:    orbitCamera->moveUp();    break;
            case GLFW_KEY_DOWN:  orbitCamera->moveDown();  break;
            default: return false;
        }
        program->loadStandardCamera(orbitCamera->getCamera());
        return true;
    });
    
    subscribeToEvent(events::EVENT_CODE_RESIZED, [&](events::Event event) {
        auto cam = orbitCamera->getCamera();
        cam.setAspectRatio(float(state.window->getWidth()) / float(state.window->getHeight()));
        program->loadStandardCamera(cam);
        return false;
    });
    
    
    // Prepare test geometry
    
    scene = new Scene();
    scene->geometryPool = { Geometry(5) };
    scene->modelPool = { { 0, Transform() } };
    
    scene->geometryPool[0].addEdit(primitives::Sphere::createEdit(opAdd, Transform(), 1));
    scene->geometryPool[0].addEdit(primitives::Sphere::createEdit(opAdd, Transform({ 1, -1, 0 }), 0.7));
    scene->geometryPool[0].addEdit(primitives::Sphere::createEdit(opAdd, Transform({ -1, -1, 0 }), 0.7));
    
    scene->geometryPool[0].addEdit(primitives::Sphere::createEdit(opAdd, Transform({ 0, -4, 0 }), 1.2));
    
    volume = buildVolumeForGeometry(scene->geometryPool[0]);
    
    // for (uint32 z = 0; z < volume->getVoxelCount(); ++z) {
    //     std::cout << "\n\n";
    //     for (uint32 y = 0; y < volume->getVoxelCount(); ++y) {
    //         std::cout << "[ ";
    //         for (uint32 x = 0; x < volume->getVoxelCount(); ++x) {
    //             auto v = volume->getVoxel(x, y, z).sdfValue;
    //             std::cout
    //                 << std::setprecision(5)
    //                 << ((v <= 0) ? "#" : " ");
    //             std::cout << " ";
    //         }
    //         std::cout << " ]\n";
    //     }
    // }
    
    volumeDistanceTexture = new gl::Texture3D(volume->getVoxelCount(), volume->getVoxelCount(), volume->getVoxelCount());
    volumeColorTexture    = new gl::Texture3D(volume->getVoxelCount(), volume->getVoxelCount(), volume->getVoxelCount());

    // program->uniform("distanceVolume", 1, *volumeDistanceTexture);
    // program->uniform("colorVolume", 2, *volumeColorTexture);
    
    program->uniform("voxelSize", volume->getVoxelSize());
    program->uniform("voxelCount", volume->getVoxelCount());
    
    // init opengl state for rendering

    program->use();
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glClearColor(0, 0, 0, 1);
    return true;
}

void Application::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6*6); // cube has 6 sides and each side has 6 verticies
}

bool Application::finalize()
{
    delete volumeDistanceTexture;
    delete volumeColorTexture;
    delete scene;
    delete volume;
    delete program;
    delete orbitCamera;
    return true;
}

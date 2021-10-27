#include <evaluator.h>

#include <types/primitives.h>
#include <types/scene.h>
#include <types/volume.h>

#include <RenderBase/rbapp.h>
#include <RenderBase/tools/camera.h>
#include <RenderBase/tools/random.h>

#include <iostream>
#include <iomanip>

using namespace std;
using namespace rb;

// scene state
Scene* scene;
Volume* volume;
OrbitCameraController* orbitCamera;

// gl things
gl::Program* program;
// gl::Texture3D* volumeTexture;
GLuint vao;

GLuint volumeTextureId;

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
    
    // Prepare camera + its controls
    
    orbitCamera = new OrbitCameraController(Camera({0, 0, -10}));
    orbitCamera->getCamera().setAspectRatio(float(state.window->getWidth()) / float(state.window->getHeight()));
    state.eventDispatcher->subscribeToEvent(events::EVENT_CODE_KEY_PRESSED, orbitCamera, [&](events::Event event) {
        switch (event.data.u16[0])
        {
            case GLFW_KEY_LEFT:  orbitCamera->moveLeft();  break;
            case GLFW_KEY_RIGHT: orbitCamera->moveRight(); break;
            case GLFW_KEY_UP:    orbitCamera->moveUp();    break;
            case GLFW_KEY_DOWN:  orbitCamera->moveDown();  break;
            case GLFW_KEY_W:     orbitCamera->zoomIn();    break;
            case GLFW_KEY_S:     orbitCamera->zoomOut();   break;
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
    scene->geometryPool = { Geometry(8) };
    scene->modelPool = { { 0, Transform() } };
    scene->geometryPool[0].addEdit(primitives::Sphere::createEdit());
    
    // glm::vec3 min = glm::vec3{-2, -2, -2};
    // glm::vec3 max = glm::vec3{2, 2, 2};
    // for (int i = 0; i < 100; ++i) {
    //     scene->geometryPool[0].addEdit(primitives::Sphere::createEdit(opAdd, Transform(randomPosition(min, max)), randomFloat(0.001, 0.7)));
    // }
        
    volume = evaluator::buildVolumeForGeometry(scene->geometryPool[0]);
    
    // std::cout << "Volume(" << volume->getVoxelCount() << ", " << volume->getVoxelSize() << ")\n";
    // for (uint32 z = 0; z < volume->getVoxelCount(); ++z) {
    //     std::cout << "\n" << float32(z) / (volume->getVoxelSize() * volume->getVoxelCount()) << "\n";
    //     for (uint32 y = 0; y < volume->getVoxelCount(); ++y) {
    //         std::cout << "[ ";
    //         for (uint32 x = 0; x < volume->getVoxelCount(); ++x) {
    //             auto v = volume->getVoxel(x, y, z).getSDFVal();
    //             std::cout
    //                 << std::setprecision(5)
    //                 << ((v <= 0) ? "#" : " ");
    //             std::cout << " ";
    //         }
    //         std::cout << " ]\n";
    //     }
    // }
    
    // Create the volume texture ...
    //
    // This and initiation of Volume instance in evaluator.cpp will be abstracted away to new "Volume" object
    // which will prepare the texture image and dispatch the evaluator compute shader
    glCreateTextures(GL_TEXTURE_3D, 1, &volumeTextureId);
    glTextureParameteri(volumeTextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(volumeTextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(volumeTextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(volumeTextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameteri(volumeTextureId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    
    float volumeBorder[] = { 1.0f, 1.0f, 0.0f, volume->getVoxelSize() * 0.5f };
    glTextureParameterfv(volumeTextureId, GL_TEXTURE_BORDER_COLOR, volumeBorder);
    glBindTextureUnit(1, volumeTextureId);
    glBindTexture(GL_TEXTURE_3D, volumeTextureId);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, volume->getVoxelCount(), volume->getVoxelCount(), volume->getVoxelCount(), 0, GL_RGBA, GL_FLOAT, volume->getRawDataPointer());
    
    // load uniform variables to the program
    program->uniform("distanceVolume", volumeTextureId);
    program->uniform("voxelSize", volume->getVoxelSize());
    program->uniform("voxelCount", volume->getVoxelCount());
    program->loadStandardCamera(orbitCamera->getCamera());
    
    // set up opengl state for rendering
    program->use();
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_CULL_FACE);
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glClearColor(0.2, 0.3, 0.7, 1);
    return true;
}

void Application::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6*6); // cube has 6 sides and each side has 6 verticies
}

bool Application::finalize()
{
    glDeleteTextures(1, &volumeTextureId);
    // delete volumeTexture;
    delete scene;
    delete volume;
    delete program;
    delete orbitCamera;
    return true;
}

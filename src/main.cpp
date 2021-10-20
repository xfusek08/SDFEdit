
#include <evaluator.h>

#include <types/scene.h>
#include <types/volume.h>
#include <types/primitives.h>

#include <RenderBase/rbapp.h>

using namespace std;
using namespace rb;

// scene state
Scene* scene;
Volume* volume;

// gl things
gl::Program* program;
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
    
    Geometry geometry = Geometry(8); // lets have low-level resolution for now
    Model    model = { 0, Transform() };
    
    geometry.addEdit(primitives::Sphere::createEdit());
    
    scene = new Scene();
    scene->geometryPool = { geometry };
    scene->modelPool    = { model };
    
    volume = buildVolumeForGeometry(scene->geometryPool[0]);
    
    // gpu data initiation
    
    program = new gl::Program(
        new gl::Shader(GL_VERTEX_SHADER, RESOURCE_SHADERS_SCREEN_QUAD_VS),
        new gl::Shader(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_SDF_BRICK_FS)
    );
    
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    program->use();
    return true;
}

void Application::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES,0,6);
}

bool Application::finalize()
{
    delete scene;
    delete program;
    delete volume;
    return true;
}
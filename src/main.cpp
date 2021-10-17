
#include <RenderBase/rbapp.h>

using namespace std;
using namespace rb;

gl::Program* program;
GLuint vao;

bool Application::init()
{
    subscribeToEvent(events::EVENT_CODE_KEY_PRESSED, [&](events::Event event) {
        if (event.data.u16[0] == GLFW_KEY_ESCAPE) {
            state.status = app::Status::Exited;
            return true;
        }
        return false;
    });
    
    program = new gl::Program(
        new gl::Shader(GL_VERTEX_SHADER, RESOURCE_SHADERS_SCREEN_QUAD_VS),
        new gl::Shader(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_RAY_MARCH_FS)
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
   delete program;
   return true;
}

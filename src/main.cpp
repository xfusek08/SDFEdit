
#include <RenderBase/rbapp.h>

using namespace std;
using namespace rb;

string const vsSrc = R".(
    #version 460 core
    out vec3 vColor;
    void main(){
        if(gl_VertexID == 0){gl_Position = vec4(0,0,0,1);vColor = vec3(1,0,0);}
        if(gl_VertexID == 1){gl_Position = vec4(1,0,0,1);vColor = vec3(0,1,0);}
        if(gl_VertexID == 2){gl_Position = vec4(0,1,0,1);vColor = vec3(0,0,1);}
    }
).";

string const fsSrc = R".(
    #version 460 core
    in vec3 vColor;
    layout(location=0)out vec4 fColor;
    void main(){
        fColor = vec4(vColor,1);
    }
).";

gl::Program* program;
GLuint vao;

bool Application::init()
{
    state.eventDispatcher->subscribeToEvent(events::EVENT_CODE_KEY_PRESSED, this, [&](events::Event event) {
        if (event.data.u16[0] == GLFW_KEY_ESCAPE) {
            state.status = app::Status::Exited;
            return true;
        }
        return false;
    });
    
    program = new gl::Program(
        new gl::Shader(GL_VERTEX_SHADER, vsSrc),
        new gl::Shader(GL_FRAGMENT_SHADER, fsSrc)
    );
    
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    program->use();
    return true;
}

void Application::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES,0,3);
}

bool Application::finalize()
{
   delete program;
   return true;
}


#include <visualization/AxisVT.h>

using namespace std;
using namespace rb;

AxisVT::AxisVT() :
    program(
        make_shared<gl::Shader>(GL_VERTEX_SHADER, RESOURCE_SHADERS_AXIS_VS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_AXIS_FS)
    )
{
    vector<glm::f32> verticies =  {
        -1.0,  0.0,  0.0,
         1.0,  0.0,  0.0,
         0.0, -1.0,  0.0,
         0.0,  1.0,  0.0,
         0.0,  0.0, -1.0,
         0.0,  0.0,  1.0,
    };

    vector<glm::u32> indices = {
        0,1,
        2,3,
        4,5
    };
    
    vector<glm::f32> colors = {
        0.6f, 0.2f, 0.2f, // negative x - dark red
        1.f, 0.f, 0.f,    // positive x - red
        0.2f, 0.6f, 0.2f, // negative y - dark green
        0.f, 1.f, 0.f,    // negative y - green
        0.f, 0.f, 1.f,    // negative z - dark blue
        0.5f, 0.5f, 1.f,  // positive z - blue
    };
    
    vertexBuffer = make_unique<gl::Buffer>(verticies, GL_STATIC_DRAW);
    indexBuffer  = make_unique<gl::Buffer>(indices, GL_STATIC_DRAW);
    
    vertexArray  = make_unique<gl::VertexArray>();
    vertexArray->addAttrib(*vertexBuffer, 0, 3, GL_FLOAT);
    vertexArray->addElementBuffer(*indexBuffer);
    
    program.uniform("scaleFactor", 10);
}

void AxisVT::prepare(const Scene& scene)
{
    // camera update
    auto cam = scene.cameraController->getCamera();
    if (cam.dirtyFlag) {
        program.loadStandardCamera(cam);
    }
}

void AxisVT::render(const Scene& scene)
{
    glEnable(GL_DEPTH_TEST);
    
    program.use();
    vertexArray->bind();
    glDrawArrays(GL_LINES, 0, indexBuffer->getSize());
    
    glDisable(GL_DEPTH_TEST);
}

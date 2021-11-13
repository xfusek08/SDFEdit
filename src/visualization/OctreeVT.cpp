
#include <visualization/OctreeVT.h>

using namespace std;
using namespace rb;

OctreeVT::OctreeVT() :
    program(
        make_shared<gl::Shader>(GL_VERTEX_SHADER,   RESOURCE_SHADERS_NODE_VS),
        make_shared<gl::Shader>(GL_GEOMETRY_SHADER, RESOURCE_SHADERS_NODE_GS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_NODE_FS)
    )
{
    // tmp: Prepare fixed data
    // -----------------------
    
    float vertices[] = { // model coordinates and level 4 floats per vertex
        0,0,0,0, // this should be unit cube center at origin
        1,1,1,1,
    };
    vertexBuffer = make_unique<gl::Buffer>(2 * 4 * sizeof(float), vertices, GL_STATIC_DRAW);
    vertexArray  = make_unique<gl::VertexArray>();
    vertexArray->addAttrib(*vertexBuffer, 0, 4, GL_FLOAT);
}

void OctreeVT::prepare(const AppState& appState)
{
}

void OctreeVT::render(const AppState& appState)
{
    // camera update
    auto cam = appState.cameraController->getCamera();
    if (cam.dirtyFlag) {
        program.loadStandardCamera(cam);
    }


    // TODO: update vertex buffer when model is dirty
    
    // tmp debug implementation
    // ------------------
    
    program.use();
    vertexArray->bind();
    glDrawArrays(GL_POINTS, 0, 2);
}


#include <visualization/OctreeWireframeVT.h>

using namespace std;
using namespace rb;

OctreeWireframeVT::OctreeWireframeVT() :
    octreeWireFrameProgram(
        make_shared<gl::Shader>(GL_VERTEX_SHADER,   RESOURCE_SHADERS_NODE_VS),
        make_shared<gl::Shader>(GL_GEOMETRY_SHADER, RESOURCE_SHADERS_NODE_GS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_NODE_FS)
    )
{
    vertexArray = make_unique<gl::VertexArray>();
}

void OctreeWireframeVT::prepare(const Scene& scene)
{
    // camera update
    auto cam = scene.cameraController->getCamera();
    if (cam.dirtyFlag) {
        octreeWireFrameProgram.loadStandardCamera(cam);
    }
}

void OctreeWireframeVT::render(const Scene& scene)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    octreeWireFrameProgram.use();
    
    // NOTE: renders first geometry
    
    const auto geometry = scene.geometryPool[0];
    vertexArray->bind();
    vertexArray->addAttrib(*(geometry->octree->vertexBuffer), 0, 4, GL_FLOAT);
    glDrawArrays(GL_POINTS, 0, geometry->octree->nodeCount);
}


#include <renderer.h>
#include <evaluator.h>

#include <RenderBase/logging.h>

using namespace std;
using namespace rb;

AppStateRenderer::AppStateRenderer() :
    program(
        make_shared<gl::Shader>(GL_VERTEX_SHADER, RESOURCE_SHADERS_SDF_BRICK_VS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_SDF_BRICK_FS)
    )
{
  
};

AppStateRenderer::~AppStateRenderer()
{
}

void AppStateRenderer::prepare()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glClearColor(0.2, 0.3, 0.7, 1);

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

void AppStateRenderer::renderState(const AppState& appState)
{
    program.use();
    
    // camera update
    auto cam = appState.cameraController->getCamera();
    if (cam.dirtyFlag) {
        program.loadStandardCamera(cam);
        cam.dirtyFlag = false;
    }
    
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // TODO: initiate batch rendering of models stored in buffer which will be evaluated before calling this method
    // and its id wii be stored in given appState
    
    // NOTE: for now only geometries are rendered
    for (const auto& geometry : appState.geometryPool->getItems()) {
        const auto& evaluatedGeometry = *geometry.evaluatedGeometry;
        
        glBindImageTexture(
            1,                      // Texture unit
            evaluatedGeometry.volumeTexture->getGlID(), // Texture name
            0,                      // Level of Mip Map
            GL_FALSE,               // Layered (false)
            0,                      // Specify layer if Layered is GL_FALSE
            GL_READ_ONLY,           // access
            GL_RGBA32F              // format
        );
        
        // we have only one geometry to render
        // program.uniform("distanceVolume", geometry.volumeTextrue);
        program.uniform("voxelSize",  evaluatedGeometry.voxelSize);
        program.uniform("voxelCount", evaluatedGeometry.voxelCount);
        glDrawArrays(GL_TRIANGLES, 0, 6*6); // cube has 6 sides and each side has 6 verticies
    }
}

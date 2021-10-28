
#include <components/SceneRenderer.h>

using namespace std;
using namespace rb;

SceneRenderer::SceneRenderer() :
    program({
        new gl::Shader(GL_VERTEX_SHADER, RESOURCE_SHADERS_SDF_BRICK_VS),
        new gl::Shader(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_SDF_BRICK_FS)
    })
{
  
};

void SceneRenderer::prepare(const ModelPool& modelPool, const GeometryPool& geometrypool, Camera& camera)
{
    program.use();
    
    if (camera.dirtyFlag) {
        program.loadStandardCamera(camera);
        camera.dirtyFlag = false;
    }
    
    // TODO: prepare buffers -> buffers should have valid data loaded already
    
    // old implementation of rendering
    
    // we have only one geometry to render
    program.uniform("distanceVolume", geometrypool.volumeTextureId());
    program.uniform("voxelSize",      geometrypool.getVolume().getVoxelSize());
    program.uniform("voxelCount",     geometrypool.getVolume().getVoxelCount());

    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_CULL_FACE);
    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glClearColor(0.2, 0.3, 0.7, 1);
}

void SceneRenderer::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6*6); // cube has 6 sides and each side has 6 verticies
}
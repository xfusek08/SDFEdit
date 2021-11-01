
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
    // TODO: prepare batch rendering for brick pool given by geometry pool
    
    
    
    // NOTE: temp implementation with single flat 3D texture per geometry
    // NOTE: very sub-optimal temp solution
    toRenderGeometry = geometrypool.getEvaluatedGeometries();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glClearColor(0.2, 0.3, 0.7, 1);

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

void SceneRenderer::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto& geometry : toRenderGeometry) {
        
        glBindImageTexture(
            1,                      // Texture unit
            geometry.volumeTextrue, // Texture name
            0,                      // Level of Mip Map
            GL_FALSE,               // Layered (false)
            0,                      // Specify layer if Layered is GL_FALSE
            GL_READ_ONLY,           // access
            GL_RGBA32F              // format
        );
        
        // we have only one geometry to render
        program.uniform("distanceVolume", geometry.volumeTextrue);
        program.uniform("voxelSize",      geometry.voxelSize);
        program.uniform("voxelCount",     geometry.voxelCount);
        glDrawArrays(GL_TRIANGLES, 0, 6*6); // cube has 6 sides and each side has 6 verticies
    }
}

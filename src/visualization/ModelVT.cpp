
#include <visualization/ModelVT.h>

#include <numeric>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace rb;

inline const vector<uint32> generateToRenderIndices(const Model& model, uint32 division)
{
    
    // TODO: run algorithm where we trace rays on gpu from camera to scene and count hitted bricks for the model
    // NOTE: for now just copy indices of particular level
    
    const auto& level = model.geometry->octree->levels[division % model.geometry->octree->levels.size()];
    // RB_DEBUG("division: " << (division % model.geometry->octree->levels.size()));
    auto res = vector<uint32>(level.nodeCount);
    iota(res.begin(), res.end(), level.startNode);
    return move(res);
}

ModelVT::ModelVT()
    : renderProgram(
        make_shared<gl::Shader>(GL_VERTEX_SHADER,   RESOURCE_SHADERS_MODEL_VS),
        make_shared<gl::Shader>(GL_GEOMETRY_SHADER, RESOURCE_SHADERS_MODEL_GS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_MODEL_FS)
        // make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_MODEL_FILL_BRICK_FS)
    )
{}

void ModelVT::prepare(const Scene& scene)
{
    auto cam = scene.cameraController->getCamera();
    if (cam.dirtyFlag) {
        renderProgram.loadStandardCamera(cam);
    }
        
    for (auto& batch : geometryBatches) {
        batch.second.transforms.clear();
        batch.second.toRenderNodes.clear();
    }
    
    // prepare batches
    // TODO: update only models and geometries which are dirty?
    //       or when camera changed
    for (int i = 0; i < scene.models.size(); ++i) {
        const auto& model = scene.models[i];
        const auto  key   = model.geometry.get();
        
        if (!model.geometry->octree) {
            RB_WARNING("geometry of model " << i << " is not evaluated");
            continue;
        }
        
        auto& currentBatch = geometryBatches[key];
        
        currentBatch.transforms.push_back(model.transform.getTransform());
        
        // calculate indices of nodes which will be rendered
        auto toRenderIndices = generateToRenderIndices(model, scene.division);
        for (auto nodeIndex : toRenderIndices) {
            currentBatch.toRenderNodes.push_back({nodeIndex, i});
        }
    }
    
    for (auto& [geometryPrt, batch] : geometryBatches) {
        
        // add nodes to be rendered ino
        if (batch.vao == nullptr) {
            batch.vao = make_unique<gl::VertexArray>();
        
            batch.transformBuffer = make_unique<gl::Buffer>(batch.transforms, GL_DYNAMIC_DRAW);
            batch.toRenderNodesBuffer = make_unique<gl::Buffer>(batch.toRenderNodes, GL_DYNAMIC_DRAW);
            
            batch.vao->addAttrib(*batch.toRenderNodesBuffer, 0, 1, GL_UNSIGNED_INT, 2);
            batch.vao->addAttrib(*batch.toRenderNodesBuffer, 1, 1, GL_UNSIGNED_INT, 2, 4);
        } else {
            batch.transformBuffer->setData(batch.transforms);
            batch.toRenderNodesBuffer->setData(batch.toRenderNodes);
        }
    }
}

void ModelVT::render(const Scene& scene)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    renderProgram.use();
    for (auto const& [geometryPrt, batch] : geometryBatches) {
        
        // attach buffers specific for current geometry/batch
        batch.vao->bind();
        renderProgram.uniform("TranslationsBlock", *batch.transformBuffer, 4);
        geometryPrt->octree->nodeBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
        geometryPrt->octree->nodeDataBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
        geometryPrt->octree->vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);
        geometryPrt->octree->brickPool->bind();
        
        glDrawArrays(GL_POINTS, 0, batch.toRenderNodes.size());
    }
    
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

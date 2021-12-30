
#include <visualization/ModelVT.h>

#include <numeric>
#include <glm/gtx/string_cast.hpp>

#include <RenderBase/asserts.h>

using namespace std;
using namespace rb;

inline const vector<uint32> generateToRenderIndices(const Model& model, uint32 division)
{
    
    // TODO: run algorithm where we trace rays on gpu from camera to scene and count hitted bricks for the model
    // NOTE: for now just copy indices of particular level
    auto levelIndex = division % model.geometry->octree->levels.size();
    
    const auto& level = model.geometry->octree->levels[levelIndex];
    
    vector<uint32> res;
    res.reserve(level.bricksInLevel);
    
    auto nodes = model.geometry->octree->getNodes();
    uint32 cnt = 0;
    for (int i = level.startNode; i < level.startNode + level.nodeCount; ++i) {
        if (nodes[i].hasBrick()) {
            res.push_back(i);
            cnt++;
        }
        // if (cnt == 1) {
        //     break;
        // }
    }
    
    // RB_DEBUG("Printing " << res.size() << " at level " << levelIndex);
    return move(res);
}

ModelVT::ModelVT()
    : renderProgram(
        make_shared<gl::Shader>(GL_VERTEX_SHADER,   RESOURCE_SHADERS_MODEL_VS),
        make_shared<gl::Shader>(GL_GEOMETRY_SHADER, RESOURCE_SHADERS_MODEL_GS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_MODEL_FS)
    ),
    brickShellProgram(
        make_shared<gl::Shader>(GL_VERTEX_SHADER,   RESOURCE_SHADERS_MODEL_VS),
        make_shared<gl::Shader>(GL_GEOMETRY_SHADER, RESOURCE_SHADERS_MODEL_GS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_MODEL_FILL_BRICK_FS)
    )
{}

void ModelVT::init(shared_ptr<Scene> scene)
{
    geometryBatches.clear();
}

void ModelVT::prepare(const Scene& scene)
{
    auto cam = scene.cameraController->getCamera();
    if (cam.dirtyFlag) {
        renderProgram.loadStandardCamera(cam);
        brickShellProgram.loadStandardCamera(cam);
    }
        
    transforms.clear();
    materials.clear();
    for (auto& batch : geometryBatches) {
        batch.second.toRenderNodes.clear();
    }
    
    // prepare batches
    // TODO: update only models and geometries which are dirty?
    //       or when camera changed
    for (int i = 0; i < scene.models.size(); ++i) {
        const auto& model = scene.models[i];
        const auto  key   = model.geometry.get();
        
        if (!model.geometry->octree) {
            RB_WARNING("geometry of model " << i << " is not evaluated .. skipping");
            continue;
        }
        
        auto& currentBatch = geometryBatches[key];
        
        transforms.push_back(model.transform.getTransform());
        materials.push_back(model.material);
        
        // calculate indices of nodes which will be rendered
        auto toRenderIndices = generateToRenderIndices(model, scene.division);
        for (auto nodeIndex : toRenderIndices) {
            currentBatch.toRenderNodes.push_back({nodeIndex, i});
        }
    }
    
    if (transformBuffer == nullptr) {
        transformBuffer = make_unique<gl::Buffer>(transforms, GL_DYNAMIC_DRAW);
        materialsBuffer = make_unique<gl::Buffer>(materials, GL_DYNAMIC_DRAW);
    } else {
        transformBuffer->setData(transforms);
        materialsBuffer->setData(materials);
    }
    
    for (auto& [geometryPrt, batch] : geometryBatches) {
        // add nodes to be rendered ino
        if (batch.vao == nullptr) {
            batch.vao = make_unique<gl::VertexArray>();
            batch.toRenderNodesBuffer = make_unique<gl::Buffer>(batch.toRenderNodes, GL_DYNAMIC_DRAW);
            batch.vao->addAttrib(*batch.toRenderNodesBuffer, 0, 1, GL_UNSIGNED_INT, 2);
            batch.vao->addAttrib(*batch.toRenderNodesBuffer, 1, 1, GL_UNSIGNED_INT, 2, 4);
        } else {
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
    
    
    auto& prg =renderProgram;
    
    auto render = [&](gl::Program& prg, uint cnt = 0) {
        prg.use();
        for (auto const& [geometryPrt, batch] : geometryBatches) {

            // attach buffers specific for current geometry/batch
            batch.vao->bind();
            geometryPrt->octree->nodeBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
            geometryPrt->octree->nodeDataBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
            geometryPrt->octree->vertexBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);

            // geometryPrt->octree->brickPool->bind(1, GL_READ_ONLY);
            
            glBindTexture(GL_TEXTURE_3D, geometryPrt->octree->brickPool->brickAtlas->getGlID());
            
            glBindImageTexture(
                1,                        // Texture unit
                geometryPrt->octree->brickPool->brickAtlas->getGlID(), // Texture name
                0,                        // Level of Mip Map
                GL_FALSE,                 // Layered (false)
                0,                        // Specify layer if Layered is GL_FALSE
                GL_READ_ONLY,             // access
                GL_R32F                   // format
            );
            
            prg.uniform("brickAtlas", uint32(1));
            prg.uniform("TranslationsBlock", *transformBuffer, 4);
            prg.uniform("MaterialBlock", *materialsBuffer, 5);
            
            prg.uniform("brickAtlasScale",     geometryPrt->octree->brickPool->getAtlasScale());
            prg.uniform("brickAtlasVoxelSize", geometryPrt->octree->brickPool->getAtlasVoxelSize());
            prg.uniform("brickAtlasStride",    geometryPrt->octree->brickPool->getAtlasStride());
            
            // debug wars
            auto setVar = [&] (string ident) {
                float val =scene.vars.getFloat(ident);
                prg.uniform(ident.c_str(), val);
            };
            setVar("a");
            setVar("b");
            setVar("c");
            setVar("d");
            
            glDrawArrays(GL_POINTS, 0, cnt > 0 ? cnt : batch.toRenderNodes.size());
        }
    };
    
    if (scene.vars.getBool("showBoxes")) {
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        render(brickShellProgram);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        render(renderProgram);
    }
}

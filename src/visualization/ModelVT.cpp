
#include <visualization/ModelVT.h>

#include <numeric>
#include <glm/gtx/string_cast.hpp>

#include <RenderBase/asserts.h>

using namespace std;
using namespace rb;

inline const vector<uint32> generateToRenderIndices(const Model& model, Scene& scene)
{
    auto extractLevelNodesIndex = [&model](uint32 levelIndex) {
        const auto& level = model.geometry->octree->levels[glm::min(uint32(model.geometry->octree->levels.size() - 1), levelIndex)];
        const auto& nodes = model.geometry->octree->getNodes();
        vector<uint32> res;
        res.reserve(level.bricksInLevel);
        for (int i = level.startNode; i < level.startNode + level.nodeCount; ++i) {
            if (nodes[i].hasBrick()) {
                res.push_back(i);
            }
        }
        return move(res);
    };
    
    const auto& camera = scene.cameraController->getCamera();
    auto distance = glm::distance(model.transform.position, camera.getPosition());
    if (distance > camera.getFarPlaneDistance()) {
        return {};
    }
    
    auto division = *scene.vars->addOrGet<uint32>("maxDivisions", 3);
    auto step = (camera.getFarPlaneDistance() * 0.1) / float32(division);
    uint32 level = division - glm::min(division, uint32(glm::floor(distance / step)));
    return move(extractLevelNodesIndex(level));
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

void ModelVT::prepare(Scene& scene)
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
        auto toRenderIndices = generateToRenderIndices(model, scene);
        
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

void ModelVT::render(Scene& scene)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    auto& prg = renderProgram;
    
    uint32* boxesRendered  = scene.vars->addOrGet<uint32>("boxesRendered", 0);
    uint32* modelsRendered = scene.vars->addOrGet<uint32>("modelsRendered", 0);
    uint32* geometriesRendered = scene.vars->addOrGet<uint32>("geometriesRendered", 0);
    
    *boxesRendered = 0;
    *modelsRendered = transforms.size();
    *geometriesRendered = geometryBatches.size();
    
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
            
            // glBindImageTexture(
            //     1,                        // Texture unit
            //     geometryPrt->octree->brickPool->brickAtlas->getGlID(), // Texture name
            //     0,                        // Level of Mip Map
            //     GL_FALSE,                 // Layered (false)
            //     0,                        // Specify layer if Layered is GL_FALSE
            //     GL_READ_ONLY,             // access
            //     GL_R32F                   // format
            // );
            
            prg.uniform("brickAtlas", uint32(1));
            prg.uniform("TranslationsBlock", *transformBuffer, 4);
            prg.uniform("MaterialBlock", *materialsBuffer, 5);
            
            prg.uniform("brickAtlasScale",     geometryPrt->octree->brickPool->getAtlasScale());
            prg.uniform("brickAtlasVoxelSize", geometryPrt->octree->brickPool->getAtlasVoxelSize());
            prg.uniform("brickAtlasStride",    geometryPrt->octree->brickPool->getAtlasStride());
            
            uint32 toRenderCount = cnt > 0 ? cnt : batch.toRenderNodes.size();
            *boxesRendered += toRenderCount;
            
            // TODO: use instanced rendering of cubes instead of generating cubes in geometry shader
            glDrawArrays(GL_POINTS, 0, toRenderCount);
        }
    };
    
    if (*scene.vars->addOrGet<bool>("showBoxes", false)) {
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        render(brickShellProgram);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        render(renderProgram);
    }
}

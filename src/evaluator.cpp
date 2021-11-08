
#include <evaluator.h>

#include <iomanip>
#include <iostream>

using namespace std;
using namespace rb;

#define BOUNDING_OFFSET 0.0f

AppStateEvaluator::AppStateEvaluator() :
    program(make_shared<gl::Shader>(GL_COMPUTE_SHADER, RESOURCE_SHADERS_EVALUATE_GEOMETRY_COMP))
{
    
}

std::unique_ptr<AppState> AppStateEvaluator::evaluateState(std::unique_ptr<AppState> oldState)
{
    // Evaluate Geometry pool
    // ----------------------
    
    // TODO: delete deleted geometries
    // TODO: create new geometries
    // TODO: update changed geometries
    
    for (int i = 0; i < oldState->geometryPool->getItems().size(); ++i) {
        if (oldState->geometryPool->getItemState(0) != ItemState::clean) {
            Geometry& geometry = oldState->geometryPool->getItem(i);
            geometry.evaluatedGeometry = evaluateGeometry(geometry, geometry.evaluatedGeometry);
        }
    }
    
    return move(oldState);
}

std::shared_ptr<EvaluatedGeometry> AppStateEvaluator::evaluateGeometry(const Geometry& geometry, std::shared_ptr<EvaluatedGeometry> oldGeometry)
{
    
    // 1.  Compute basic metadata about bounding voxelized volume (shader uniforms)
    // -----------------------------------------------------------------------------
    
    // Nearest upper number of voxels divisible by 8 in one edge of volume.
    glm::u32 voxelCount = glm::round(glm::f32(geometry.getResolution()) / 8.0f) * 8.0f;
    
    // Unit cube around the BB is divided into geometry.resolution^3 cubical voxels.
    glm::f32 voxelSize = (geometry.getAABB().longestEdgeSize() + BOUNDING_OFFSET) / glm::f32(voxelCount - 2);
    // glm::f32 voxelSize = geometry.getAABB().longestEdgeSize() / glm::f32(voxelCount);
    
    // Primitives inside geometry will be displaced relative to the center of BB the insted of geometry origin.
    glm::vec3 primitiveCenterCorrection = -geometry.getAABB().center();
    
    
        
    // 2. update or create evaluated geometry
    // ---------------------------------------
    
    struct Edit {
        uint32    type;
        uint32    op;
        uint32    blending;
        uint32    _padding1;
        glm::vec3 pos;
        float     _padding2;
        glm::vec4 data;
    };
    
    std::vector<Edit> editsData;
    for (auto e : geometry.getEdits()) {
        editsData.push_back({
            e.primitiveType,
            e.operation,
            e.blending,
            0,
            e.transform.position,
            0,
            e.primitiveData
        });
    }
    
    std::shared_ptr<EvaluatedGeometry> resultEvaluatedGeometry = oldGeometry;

    if (resultEvaluatedGeometry == nullptr) {
        
        // New evaluated geometry instance
        // --------------------------------
        
        resultEvaluatedGeometry = make_shared<EvaluatedGeometry>();
        resultEvaluatedGeometry->editsBuffer   = make_unique<gl::Buffer>(editsData, GL_DYNAMIC_DRAW);
        resultEvaluatedGeometry->volumeTexture = make_unique<gl::Texture3D>();
        
        // Init geometry texture
        // ----------------------
        
        glTextureParameteri(resultEvaluatedGeometry->volumeTexture->getGlID(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(resultEvaluatedGeometry->volumeTexture->getGlID(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(resultEvaluatedGeometry->volumeTexture->getGlID(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(resultEvaluatedGeometry->volumeTexture->getGlID(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTextureParameteri(resultEvaluatedGeometry->volumeTexture->getGlID(), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        float volumeBorder[] = { 0.0f, 0.0f, 0.0f, voxelSize * 0.5f };
        glTextureParameterfv(resultEvaluatedGeometry->volumeTexture->getGlID(), GL_TEXTURE_BORDER_COLOR, volumeBorder);
        glBindTexture(GL_TEXTURE_3D, resultEvaluatedGeometry->volumeTexture->getGlID());
        glTexImage3D(
            GL_TEXTURE_3D,
            0,
            GL_RGBA32F,
            voxelCount,
            voxelCount,
            voxelCount,
            0,
            GL_RGBA,
            GL_FLOAT,
            nullptr
        );
        
    } else {
        // update initialized geometry
        // --------------------------------
        resultEvaluatedGeometry->editsBuffer->setData(editsData);
    }
    
    resultEvaluatedGeometry->voxelCount    = voxelCount;
    resultEvaluatedGeometry->voxelSize     = voxelSize;
    
    
    // 3. Prepare and dispatch compute shader
    // ---------------------------------------
    
    program.use();
    program.uniform("centerCorrection", -geometry.getAABB().center());
    program.uniform("voxelSize", voxelSize);
    program.uniform("voxelCount", voxelCount);
    program.uniform("editCount", uint32(geometry.getEdits().size()));
    
    resultEvaluatedGeometry->editsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    
    // Prepare texture to be written to:
    glBindImageTexture(
        0,                                                 // Texture unit
        resultEvaluatedGeometry->volumeTexture->getGlID(), // Texture name
        0,                                                 // Level of Mip Map
        GL_TRUE,                                           // Layered (false) -> this needs to be true because we have 3d texture
        0,                                                 // Specify layer if Layered is GL_FALSE
        GL_WRITE_ONLY,                                     // access
        GL_RGBA32F                                         // format
    );
    
    glDispatchCompute(voxelCount / 8, voxelCount / 8, voxelCount / 8);
    
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // TODO: this maybe should be as part of render preparations
    
    return resultEvaluatedGeometry;
}

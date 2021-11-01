
#include <components/GeometryEvaluator.h>

#include <iomanip>
#include <iostream>

#define BOUNDING_OFFSET 0.0f

using namespace rb;

GeometryEvaluator::GeometryEvaluator() :
    program({ new gl::Shader(GL_COMPUTE_SHADER, RESOURCE_SHADERS_EVALUATE_GEOMETRY_COMP) })
{
    
}

// TODO: input to this function should be a pre-created buffer which content will be updated on GPU
// and the sparse texture of bricks
EvaluatedGeometry GeometryEvaluator::evaluate(const Geometry& geometry, rb::gl::SparseTexture3D& brickPool)
{
    
    // 1.  Compute basic metadata about bounding voxelized volume (shader uniforms)
    // -----------------------------------------------------------------------------
    
    // Nearest upper even number of voxel in one edge of volume.
    glm::u32 voxelCount = glm::round(glm::f32(geometry.getResolution()) / 2.0f) * 2.0f;
    
    // Unit cube around the BB is divided into geometry.resolution^3 cubical voxels.
    glm::f32 voxelSize = (geometry.getAABB().longestEdgeSize() + BOUNDING_OFFSET) / glm::f32(voxelCount - 2);
    // glm::f32 voxelSize = geometry.getAABB().longestEdgeSize() / glm::f32(voxelCount);
    
    // Primitives inside geometry will be displaced relative to the center of BB the insted of geometry origin.
    glm::vec3 primitiveCenterCorrection = -geometry.getAABB().center();
    
    
    // 2. Prepare shader outputs
    // --------------------------
    // NOTE: Now it is only one simple 3D texture containing whole volume of one geometry.
    // TODO: In the future compute shader output this will existing sparse 3d texture for individual 8x8x8 bricks
    // and a new buffer to store evaluated node octree.
        
    // glEnable(GL_TEXTURE_3D);
    GLuint volumeTexture;
    float volumeBorder[] = { 0.0f, 0.0f, 0.0f, voxelSize * 0.5f };
    glCreateTextures(GL_TEXTURE_3D, 1, &volumeTexture);
    glTextureParameteri(volumeTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(volumeTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(volumeTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(volumeTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameteri(volumeTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTextureParameterfv(volumeTexture, GL_TEXTURE_BORDER_COLOR, volumeBorder);
    
    // prepare image buffer size
    glBindTexture(GL_TEXTURE_3D, volumeTexture);
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
    
    // Prepare texture to be written to:
    glBindImageTexture(
        0,             // Texture unit
        volumeTexture, // Texture name
        0,             // Level of Mip Map
        GL_TRUE,       // Layered (false) -> this needs to be true because we have 3d texture
        0,             // Specify layer if Layered is GL_FALSE
        GL_READ_WRITE, // access
        GL_RGBA32F     // format
    );
    
    
    
    // 3. Prepare shader inputs
    // -------------------------
    // This will create shader storage buffer object to which list of geometry edits will be uploaded.
    // TODO: abstract buffer to object instance when I will know what is needed to maintain it
    // TODO: What about padding the struct?
        
    // Edits geometry buffer
    
    struct Edit {
        uint32 type;
        uint32 op;
        uint32 blending;
        uint32 padding1;
        glm::vec3 pos;
        float padding2;
        glm::vec4 data;
    };
    
    std::vector<Edit> edits;
    for (auto e : geometry.getEdits()) {
        edits.push_back({
            e.primitiveType,
            e.operation,
            e.blending,
            0,
            e.transform.position,
            0,
            e.primitiveData
        });
    }
    
    GLuint editListBuffer;
    glCreateBuffers(1, &editListBuffer);
    glNamedBufferData(editListBuffer, sizeof(Edit) * edits.size(), edits.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, editListBuffer);
    
    // Uniforms
    program.uniform("centerCorrection", -geometry.getAABB().center());
    program.uniform("voxelSize", voxelSize);
    program.uniform("voxelCount", voxelCount);
    program.uniform("editCount", uint32(geometry.getEdits().size()));
    
    
    
    // 4 Prepare compute shader work group size
    // -----------------------------------------
    // TODO: computing possible workgroup sizes should be part ouf initiation process
    
    // // Zjištění možného množství pracovních skupin na GPU:
    // GLint workGroupCounts[3];
    // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
    // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
    // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);
    // RB_DEBUG("Possible work groups: (" << workGroupCounts[0] << ", " << workGroupCounts[1] << ", " << workGroupCounts[2]<< ")");
    
    // // Zjištění maximální velikosti jedné pracovní skupiny na GPU:
    // GLint workGroupSize[3];
    // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
    // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
    // glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);
    // RB_DEBUG("Work group size: (" << workGroupSize[0] << ", " << workGroupSize[1] << ", " << workGroupSize[2]<< ")");

    // GLint maximumLocalWorkGroupSize;
    // glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maximumLocalWorkGroupSize);
    // RB_DEBUG("Maximum local work group invocations: " << maximumLocalWorkGroupSize);
    
    // Dispatch shader
    program.use();
    glDispatchCompute(voxelCount,voxelCount,voxelCount);
    
    // Wait to shared be done with rendering
    // TODO: this maybe should be as part of render preparations
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    // glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
    
    
    // // NOTE: temp checking what was generated into a texture
    // auto pixels = std::vector<float>(voxelCount*voxelCount*voxelCount*4, 5);
    // glGetTextureImage(
    //     volumeTexture,
    //     0,
    //     GL_RGBA,
    //     GL_FLOAT,
    //     pixels.size() * sizeof(float),
    //     pixels.data()
    // );
    
    // for (uint32 z = 0; z < voxelCount; ++z) {
    //     std::cout << "\n";
    //     for (uint32 y = 0; y < voxelCount; ++y) {
    //         std::cout << "[ ";
    //         for (uint32 x = 0; x < voxelCount; ++x) {
    //             uint32 index = 4*(z * voxelCount*voxelCount + y*voxelCount + x) + 3;
    //             auto v = pixels[index];
    //             std::cout
    //                 << std::setprecision(5)
    //                 << ((v <= 0) ? "#" : " ")
    //                 << " ";
                    
    //         }
    //         std::cout << " ]\n";
    //     }
    // }
    
    return {
        voxelCount,
        voxelSize,
        volumeTexture
    };
}
    
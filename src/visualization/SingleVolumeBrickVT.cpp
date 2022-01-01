
#include <visualization/SingleVolumeBrickVT.h>

// #include <iomanip>
// #include <iostream>

using namespace std;
using namespace rb;

#define BOUNDING_OFFSET 0.0f

SingleVolumeBrickVT::SingleVolumeBrickVT() :
    computeProgram(
        make_shared<gl::Shader>(GL_COMPUTE_SHADER, RESOURCE_SHADERS_EVALUATE_GEOMETRY_COMP)
    ),
    renderProgram(
        make_shared<gl::Shader>(GL_VERTEX_SHADER, RESOURCE_SHADERS_SDF_BRICK_VS),
        make_shared<gl::Shader>(GL_FRAGMENT_SHADER, RESOURCE_SHADERS_SDF_BRICK_FS)
    )
{
    glCreateVertexArrays(1, &vao);
}

void SingleVolumeBrickVT::prepare(Scene& scene)
{
    // Choose geometry to evaluate onto GPU
    // ------------------------------------
    
    auto geometry = scene.models[0].geometry;
    if (geometry->dirty) {
        
        // Compute basic metadata about bounding voxelized volume (shader uniforms)
        // -----------------------------------------------------------------------------
        
        // Nearest upper number of voxels divisible by 8 in one edge of volume.
        voxelCount = glm::round(glm::f32(geometry->getResolution()) / 8.0f) * 8.0f;
        
        // Unit cube around the BB is divided into geometry->resolution^3 cubical voxels.
        voxelSize = (geometry->getAABB().longestEdgeSize() + BOUNDING_OFFSET) / glm::f32(voxelCount - 2);
        // voxelSize = geometry->getAABB().longestEdgeSize() / glm::f32(voxelCount);
        
        // Primitives inside geometry will be displaced relative to the center of BB the insted of geometry origin.
        glm::vec3 primitiveCenterCorrection = -geometry->getAABB().center();
        
        
        
        // update or create evaluated geometry
        // ---------------------------------------
        
        struct Edit {
            uint32    type;
            uint32    op;
            float32   blending;
            uint32    _padding1;
            glm::vec3 pos;
            float     _padding2;
            glm::vec4 data;
        };
        
        std::vector<Edit> editsData;
        for (auto e : geometry->getEdits()) {
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
        
        if (!editsBuffer) {
            // Create new buffers
            editsBuffer   = make_unique<gl::Buffer>(editsData, GL_DYNAMIC_DRAW);
            volumeTexture = make_unique<gl::Texture3D>();
            
            // Init the texture
            // TODO: move this initiation into the object
            glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTextureParameteri(volumeTexture->getGlID(), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            float volumeBorder[] = { 0.0f, 0.0f, 0.0f, voxelSize * 0.5f };
            glTextureParameterfv(volumeTexture->getGlID(), GL_TEXTURE_BORDER_COLOR, volumeBorder);
            glBindTexture(GL_TEXTURE_3D, volumeTexture->getGlID());
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
            editsBuffer->setData(editsData);
        }
        
        
        
        // Dispatch compute shader
        // ------------------------
        
        computeProgram.use();
        computeProgram.uniform("centerCorrection", -geometry->getAABB().center());
        computeProgram.uniform("voxelSize", voxelSize);
        computeProgram.uniform("voxelCount", voxelCount);
        computeProgram.uniform("editCount", uint32(geometry->getEdits().size()));
        
        editsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
        // Prepare texture to be written to:
        glBindImageTexture(
            0,                        // Texture unit
            volumeTexture->getGlID(), // Texture name
            0,                        // Level of Mip Map
            GL_TRUE,                  // Layered (false) -> this needs to be true because we have 3d texture
            0,                        // Specify layer if Layered is GL_FALSE
            GL_WRITE_ONLY,            // access
            GL_RGBA32F                // format
        );
        glDispatchCompute(voxelCount / 8, voxelCount / 8, voxelCount / 8);
        
        // // DEBUG WRITE OUT
        // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        // auto pixels = std::vector<float>(voxelCount*voxelCount*voxelCount*4, 5);
        // glGetTextureImage(
        //     volumeTexture->getGlID(),
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
        
    }
    
    renderProgram.uniform("voxelSize",  voxelSize);
    renderProgram.uniform("voxelCount", voxelCount);

    auto cam = scene.cameraController->getCamera();
    if (cam.dirtyFlag) {
        renderProgram.loadStandardCamera(cam);
        cam.dirtyFlag = false;
    }
}

void SingleVolumeBrickVT::render(Scene& scene)
{
    renderProgram.use();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    
    glBindVertexArray(vao);
    
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // wait till texture is finished
    glBindImageTexture(
        1,                        // Texture unit
        volumeTexture->getGlID(), // Texture name
        0,                        // Level of Mip Map
        GL_FALSE,                 // Layered (false)
        0,                        // Specify layer if Layered is GL_FALSE
        GL_READ_ONLY,             // access
        GL_RGBA32F                // format
    );
    glDrawArrays(GL_TRIANGLES, 0, 6*6); // cube has 6 sides and each side has 6 verticies
}

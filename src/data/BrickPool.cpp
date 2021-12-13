
#include <data/BrickPool.h>

#include <RenderBase/logging.h>

#include <vector>

using namespace std;
using namespace rb;

BrickPool::BrickPool(uint32 bricksInOneDimension, uint32 brickSize, GLenum format) :
    bricksInOneDimension(bricksInOneDimension),
    brickSize(brickSize),
    format(format)
{
    brickAtlas = make_unique<gl::Texture3D>();
    glTextureParameteri(brickAtlas->getGlID(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(brickAtlas->getGlID(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTextureParameteri(brickAtlas->getGlID(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTextureParameteri(brickAtlas->getGlID(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(brickAtlas->getGlID(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(brickAtlas->getGlID(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameteri(brickAtlas->getGlID(), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    float borderDistance = 0;
    glTextureParameterfv(brickAtlas->getGlID(), GL_TEXTURE_BORDER_COLOR, &borderDistance);
    glBindTexture(GL_TEXTURE_3D, brickAtlas->getGlID());
    
    // for now voxels contains only fp distance values
    glTexImage3D(
        GL_TEXTURE_3D,
        0,
        GL_R32F,
        getAtlasEdgeSize(),
        getAtlasEdgeSize(),
        getAtlasEdgeSize(),
        0,
        format,
        GL_FLOAT,
        nullptr
    );
    
    glBindTexture(GL_TEXTURE_3D, 0);
    
    RB_INFO("Brick Pool 3D texture created: (" << bricksInOneDimension << " x " << bricksInOneDimension << " x " << bricksInOneDimension << "): ");
    RB_INFO("    Maximal Bricks:  " << (bricksInOneDimension * bricksInOneDimension * bricksInOneDimension));
    RB_INFO("    Total Bytes:     " << (bricksInOneDimension * bricksInOneDimension * bricksInOneDimension) * (brickSize*brickSize*brickSize) * 4);
}

void BrickPool::bind(GLuint textureUnit, GLenum accessFlags)
{
    glBindImageTexture(
        textureUnit,           // Texture unit index
        brickAtlas->getGlID(), // Texture name
        0,                     // Level of Mip Map
        GL_TRUE,               // Layered (false) -> this needs to be true because we have 3d texture
        0,                     // Specify layer if Layered is GL_FALSE
        accessFlags,           // access
        GL_R32F                // format
    );
}

void BrickPool::debugDraw() const
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    auto atlasWidth = getAtlasEdgeSize();
    auto pixels = vector<float32>(atlasWidth * atlasWidth * atlasWidth, 5); // each voxel should be exactly one float
    glGetTextureImage(
        brickAtlas->getGlID(),
        0,
        format,
        GL_FLOAT,
        pixels.size() * sizeof(float32),
        pixels.data()
    );
    
    auto printBrick = [&](glm::uvec3 brickCoords) {
        for (uint32 z = 0; z < brickSize; ++z) {
            for (uint32 y = 0; y < brickSize; ++y) {
                string line = "[ ";
                for (uint32 x = 0; x < brickSize; ++x) {
                    uint32 index = (brickCoords.x + x + 1) + (brickCoords.y + y + 1) * atlasWidth + (brickCoords.z + z + 1) * atlasWidth * atlasWidth;
                    auto v = pixels[index];
                    if (v <= 0) {
                        line += "#";
                    } else if (v <= 0.3) {
                        line += ".";
                    } else if (v <= 0.3) {
                        line += " ";
                    }
                    line += " ";
                }
                line += " ]";
                RB_DEBUG(line);
            }
        }
    };
    
    printBrick({ 0, 0, 0 });
    // printBrick({ 0, 0, 2 });
    // printBrick({ 0, 0, 3 });
}

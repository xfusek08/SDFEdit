
#include <data/BrickPool.h>

#include <RenderBase/logging.h>

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
        format,
        bricksInOneDimension * brickSize,
        bricksInOneDimension * brickSize,
        bricksInOneDimension * brickSize,
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
        format                 // format
    );
}

#pragma once

#include <RenderBase/gl/Texture3D.h>

#include <memory>

struct BrickPool {
    
    const uint32 brickSize;
    const uint32 bricksInOneDimension;
    const GLenum format;
    
    GLsizeiptr brickCount = 0;
    std::unique_ptr<rb::gl::Texture3D> brickAtlas = nullptr;
    
    BrickPool(
        uint32 bricksInOneDimension = 16, // 8 000 bricks cca 16.5 MB (GL_RED), 65.5 MB (GL_RGB32F)
        uint32 brickSize            = 8,
        GLenum format               = GL_RED
    );
    
    inline uint32  getMaximalBrickCount() const { return bricksInOneDimension * bricksInOneDimension * bricksInOneDimension; };
    inline uint32  getAtlasEdgeSize()     const { return (brickSize + 2) * bricksInOneDimension; };
    inline float32 getAtlasScale()        const { return float32(brickSize) / float32(getAtlasEdgeSize()); }
    inline float32 getAtlasStride()       const { return 1.0 / float32(bricksInOneDimension); } // TODO: this will be different when leaving gaps of 1 voxel between bricks in the atlas
    inline float32 getAtlasVoxelSize()    const { return 1.0 / float32(getAtlasEdgeSize()); }
    
    void bind(GLuint textureUnit = 0, GLenum accessFlags = GL_READ_WRITE);
    
    void debugDraw() const;
};

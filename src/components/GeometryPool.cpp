
#include <components/GeometryPool.h>

#include <iomanip>
#include <iostream>

using namespace std;

GeometryPool::~GeometryPool()
{
    // TODO: free up all allocated gpu memory
    
    // old tpm implementation of drawing one single geometry
    if (texId != -1) {
        glDeleteTextures(1, &texId);
    }
}

void GeometryPool::evaluateGeometries(const GeometryEvaluator& evaluator)
{
    // TODO: use evaluator evaluate all invalid geometries and micro-manage their states on gpu.
    
    // old tpm implementation of drawing one single geometry
    rb::gl::SparseTexture3D dummyTexture;
    EvaluatedGeometry evalResult = evaluator.evaluate(this->items[0], dummyTexture);
    
    if (texId != -1) {
        glDeleteTextures(1, &texId);
    }
    
    glCreateTextures(GL_TEXTURE_3D, 1, &texId);
    glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTextureParameteri(texId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    
    float volumeBorder[] = { 1.0f, 1.0f, 0.0f, evalResult.volume->getVoxelSize() * 0.5f };
    glTextureParameterfv(texId, GL_TEXTURE_BORDER_COLOR, volumeBorder);
    glBindTextureUnit(1, texId);
    glBindTexture(GL_TEXTURE_3D, texId);
    glTexImage3D(
        GL_TEXTURE_3D,
        0,
        GL_RGBA32F,
        evalResult.volume->getVoxelCount(),
        evalResult.volume->getVoxelCount(),
        evalResult.volume->getVoxelCount(),
        0,
        GL_RGBA,
        GL_FLOAT,
        evalResult.volume->getRawDataPointer()
    );
    
    volume = move(evalResult.volume);
    
    std::cout << "Volume(" << volume->getVoxelCount() << ", " << volume->getVoxelSize() << ")\n";
    for (uint32 z = 0; z < volume->getVoxelCount(); ++z) {
        std::cout << "\n" << float32(z) / (volume->getVoxelSize() * volume->getVoxelCount()) << "\n";
        for (uint32 y = 0; y < volume->getVoxelCount(); ++y) {
            std::cout << "[ ";
            for (uint32 x = 0; x < volume->getVoxelCount(); ++x) {
                auto v = volume->getVoxel(x, y, z).getSDFVal();
                std::cout
                    << std::setprecision(5)
                    << ((v <= 0) ? "#" : " ");
                std::cout << " ";
            }
            std::cout << " ]\n";
        }
    }
    
}
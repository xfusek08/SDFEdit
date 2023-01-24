#version 460 core

#define MAX_MODELS 1000 // maximum number of nodes in view to limit size of constant memory for uniform block

struct Material {
    vec3 color;
    float shininess;
};

// vertex parameters from vertex puller
layout (location = 0) in uint vertex_nodeIndex;
layout (location = 1) in uint vertex_transformIndex;

// to read data buffers
layout(std430, binding = 2) buffer  NodeDataPool   { uint nodeData[]; };
layout(std430, binding = 3) buffer  VertexBuffer   { vec4 nodeVertices[]; };
layout(std140, binding = 4) uniform TransformBlock { mat4 transforms[MAX_MODELS]; }; // world translations uniform buffer block
layout(std140, binding = 5) uniform MaterialBlock  { Material materials[MAX_MODELS]; };

// Precomputed values static for current brick to avoid loading them from memory for each fragment
out mat4  transformMatrices;
out vec3  brickAtlasShifts;
out vec3  colors;
out float shininesses;

uniform float brickAtlasStride;
uniform float brickAtlasVoxelSize;

vec3 decodeBricksCoords(uint coords) {
    return vec3(
        (coords >> 20) & 0x3FF,
        (coords >> 10) & 0x3FF,
        coords & 0x3FF
    );
}

// look up shift is translation of position from scaled ray-marcher space of the brick
// into right position in the brick texture atlas
vec3 calculateAtlasLookupShift() {
    uint encodedCoords = nodeData[vertex_nodeIndex];
    return brickAtlasStride * decodeBricksCoords(encodedCoords) + vec3(brickAtlasVoxelSize);
}

void main()
{
    vec4 vertex       = nodeVertices[vertex_nodeIndex];
    transformMatrices = transforms[vertex_transformIndex];
    brickAtlasShifts  = calculateAtlasLookupShift();
    colors            = materials[vertex_transformIndex].color;
    shininesses       = materials[vertex_transformIndex].shininess;
    gl_Position       = vertex;
}

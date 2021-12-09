#version 460 core

#ifndef BRICK_SIDE_LENGTH
    #define BRICK_SIDE_LENGTH 8  // lets have NxNxN bricks
#endif
#if BRICK_SIDE_LENGTH == 8
    #define HALF_VOXEL_SIZE 0.0625 // half of one voxel, to avoid division
#else
    #error BRICK_SIDE_LENGTH macro must have value 8 for now
#endif

#define MAX_MODELS 1000 // maximum number of modes in view to limit size of constant memory for uniform block

// vertex parameters from vertex puller
layout (location = 0) in uint vertex_nodeIndex;
layout (location = 1) in uint vertex_transformIndex;

// to read data buffers
layout(std430, binding = 3) buffer VertexBuffer    { vec4 nodeVertices[]; };
layout(std140, binding = 4) uniform TransformBlock { mat4 transforms[MAX_MODELS]; }; // world translations uniform buffer block

// Precomputed values static for current brick to avoid loading them from memory for each fragment
out uint  nodeIndexes;
out mat4  transformMatrices;
out float voxelHalfSizes;
out vec3  brickAtlasLookupShifts;


// look up shift is translation of position in ray-marcher space of the brick
// into right position in the brick texture atlas
vec3 calculateAtlasLookupShift() {
    return vec3(0, 0, 0);
}

void main()
{
    vec4 vertex            = nodeVertices[vertex_nodeIndex];
    nodeIndexes            = vertex_nodeIndex;
    voxelHalfSizes         = vertex.w * HALF_VOXEL_SIZE;
    transformMatrices      = transforms[vertex_transformIndex];
    brickAtlasLookupShifts = calculateAtlasLookupShift();
    gl_Position            = vertex;
}

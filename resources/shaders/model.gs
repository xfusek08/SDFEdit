#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

// data computed in vertex shader
in float voxelHalfSizes[];
in mat4  transformMatrices[];
in vec3  brickAtlasShifts[];
in float brickInvertedSizes[];

// various pre computed values needed by fragment shader - brick marcher
smooth out vec3 fragPos;
flat out vec4   nodeVertex;
flat out float  voxelHalfSize;
out mat4        brickToOctreeSpaceMatrix;
flat out vec3   brickAtlasShift;
flat out float  brickInvertedSize;


// F - FRONT | T - TOP  | L - left
// B - Back  | D - Down | R - Right

// front face
#define FTL(D) vec3(-D,  D, D)
#define FTR(D) vec3( D,  D, D)
#define FDL(D) vec3(-D, -D, D)
#define FDR(D) vec3( D, -D, D)

// back face
#define BTL(D) vec3(-D,  D, -D)
#define BTR(D) vec3( D,  D, -D)
#define BDL(D) vec3(-D, -D, -D)
#define BDR(D) vec3( D, -D, -D)

uniform mat4 viewProjection;

void main() {
    // output common to whore node
    mat4  transformMatrix = transformMatrices[0];
    
    nodeVertex               = gl_in[0].gl_Position;
    voxelHalfSize            = voxelHalfSizes[0];
    brickToOctreeSpaceMatrix = inverse(transformMatrix);
    brickAtlasShift          = brickAtlasShifts[0];
    brickInvertedSize        = brickInvertedSizes[0];
    
    vec3  nodePos  = nodeVertex.xyz;
    // float stepSize = nodeVertex.w * 0.468;
    float stepSize = nodeVertex.w * 0.5;
    vec4  worldPos; // tmp register
    #define EMIT_STRIP_VERTEX(shift) \
        worldPos    = transformMatrix * vec4(nodePos + shift, 1); \
        fragPos     = worldPos.xyz; \
        gl_Position = viewProjection * worldPos; \
        EmitVertex()
    
    EMIT_STRIP_VERTEX( BDL(stepSize) );
    EMIT_STRIP_VERTEX( BTL(stepSize) );
    EMIT_STRIP_VERTEX( BDR(stepSize) );
    EMIT_STRIP_VERTEX( BTR(stepSize) );
    EMIT_STRIP_VERTEX( FDR(stepSize) );
    EMIT_STRIP_VERTEX( FTR(stepSize) );
    EMIT_STRIP_VERTEX( FDL(stepSize) );
    EMIT_STRIP_VERTEX( FTL(stepSize) );
    EndPrimitive();
    
    EMIT_STRIP_VERTEX( BDR(stepSize) );
    EMIT_STRIP_VERTEX( FDR(stepSize) );
    EMIT_STRIP_VERTEX( BDL(stepSize) );
    EMIT_STRIP_VERTEX( FDL(stepSize) );
    EMIT_STRIP_VERTEX( BTL(stepSize) );
    EMIT_STRIP_VERTEX( FTL(stepSize) );
    EMIT_STRIP_VERTEX( BTR(stepSize) );
    EMIT_STRIP_VERTEX( FTR(stepSize) );
    EndPrimitive();
}

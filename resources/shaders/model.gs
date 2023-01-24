#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

uniform vec3 cameraPosition;

// data computed in vertex shader
in mat4  transformMatrices[];
in vec3  brickAtlasShifts[];
in vec3  colors[];
in float shininesses[];

// various pre computed values common to all fragments for brick marching
smooth out vec3 fragPos;
flat out mat4   brickTransformMatrix;
flat out vec3   brickCameraPosition;
flat out vec3   brickAtlasShift;
flat out vec3   color;
flat out float  shininess;

// Vertices of cube:
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

mat4 translate(vec3 translation) {
    mat4 res = mat4(1.0);
    res[3][0] = translation.x;
    res[3][1] = translation.y;
    res[3][2] = translation.z;
    return res;
}

mat4 scale(float scaling) {
    mat4 res = mat4(1.0);
    res[0][0] = scaling;
    res[1][1] = scaling;
    res[2][2] = scaling;
    return res;
}

void main() {
    // output common to whore node
    
    vec4  nodeVertex        = gl_in[0].gl_Position;
    float brickInvertedSize = 1.0 / nodeVertex.w;
    float stepSize          = nodeVertex.w * 0.5;
    vec3  brickShift        = nodeVertex.www * 0.5 - nodeVertex.xyz;
    mat4  transformMatrix   = transformMatrices[0];
    
    // add scaling to the matrix
    brickTransformMatrix = scale(brickInvertedSize) * translate(brickShift) * inverse(transformMatrix);
    brickCameraPosition  = (brickTransformMatrix * vec4(cameraPosition, 1)).xyz;
    brickAtlasShift      = brickAtlasShifts[0];
    color                = colors[0];
    shininess            = shininesses[0];
    
    vec4 worldPos; // tmp register
    #define EMIT_STRIP_VERTEX(shift)                                     \
        worldPos    = transformMatrix * vec4(nodeVertex.xyz + shift, 1); \
        fragPos     = worldPos.xyz;                                      \
        gl_Position = viewProjection * worldPos;                         \
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

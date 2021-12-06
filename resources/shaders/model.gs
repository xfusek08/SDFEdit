#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

// node pool
layout(std430, binding = 1) buffer NodePool     { uint nodes[]; };
layout(std430, binding = 2) buffer NodeDataPool { uint nodeData[]; };
layout(std430, binding = 3) buffer VertexBuffer { vec4 nodeVertices[]; };

in uint nodeIndex[];

smooth out vec3 fragPos;

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
    vec3  translate = gl_in[0].gl_Position.xyz;
    float scale     = gl_in[0].gl_Position.w;
    
    vec4  nodeVertex = nodeVertices[nodeIndex[0]];
    vec3  position   = nodeVertex.xyz + translate;
    float d          = nodeVertex.w * 0.5 * scale;
    
    vec3 finalPos;
    
    #define EMIT_STRIP_VERTEX(shift) \
        finalPos    = position + shift; \
        fragPos     = finalPos; \
        gl_Position = viewProjection * vec4(finalPos, 1); \
        EmitVertex()
    
    EMIT_STRIP_VERTEX( BDL(d) );
    EMIT_STRIP_VERTEX( BTL(d) );
    EMIT_STRIP_VERTEX( BDR(d) );
    EMIT_STRIP_VERTEX( BTR(d) );
    EMIT_STRIP_VERTEX( FDR(d) );
    EMIT_STRIP_VERTEX( FTR(d) );
    EMIT_STRIP_VERTEX( FDL(d) );
    EMIT_STRIP_VERTEX( FTL(d) );
    EndPrimitive();
        
    EMIT_STRIP_VERTEX( BDR(d) );
    EMIT_STRIP_VERTEX( FDR(d) );
    EMIT_STRIP_VERTEX( BDL(d) );
    EMIT_STRIP_VERTEX( FDL(d) );
    EMIT_STRIP_VERTEX( BTL(d) );
    EMIT_STRIP_VERTEX( FTL(d) );
    EMIT_STRIP_VERTEX( BTR(d) );
    EMIT_STRIP_VERTEX( FTR(d) );
    EndPrimitive();
}

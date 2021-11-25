#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

smooth out vec3 fragPos;

uniform vec4 brickSize;
uniform mat4 viewProjection;

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

void main() {
    vec3 position = gl_in[0].gl_Position.xyz;
    float d       = gl_in[0].gl_Position.w * 0.5;

     #define EMIT_STRIP_VERTEX(shift) \
        gl_Position = viewProjection * vec4(position + shift, 1); EmitVertex()
        
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

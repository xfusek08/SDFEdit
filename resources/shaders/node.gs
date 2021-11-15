#version 460 core

layout (points) in;
layout (line_strip, max_vertices = 16) out;

in vec4 color[];
out vec4 vcolor;

// Half edge lenght of level 0 cube
#define D_0 0.5

// 8 verticies in cube:

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
    vec3 position = gl_in[0].gl_Position.xyz;
    float d       = gl_in[0].gl_Position.w;
    vcolor        = color[0];
    
    // FIRST PRIMITIVE
    #define EMIT_LINE_VERTEX(shift) \
        gl_Position = viewProjection * vec4(position + shift, 1); \
        EmitVertex()
    
    EMIT_LINE_VERTEX( FTL(d) );
    EMIT_LINE_VERTEX( FTR(d) );
    EMIT_LINE_VERTEX( FDR(d) );
    EMIT_LINE_VERTEX( FDL(d) );
    EMIT_LINE_VERTEX( FTL(d) );
    EMIT_LINE_VERTEX( BTL(d) );
    EndPrimitive();
    
    EMIT_LINE_VERTEX( FDL(d) );
    EMIT_LINE_VERTEX( BDL(d) );
    EMIT_LINE_VERTEX( BTL(d) );
    EMIT_LINE_VERTEX( BTR(d) );
    EMIT_LINE_VERTEX( BDR(d) );
    EMIT_LINE_VERTEX( BDL(d) );
    EndPrimitive();

    EMIT_LINE_VERTEX( FTR(d) );
    EMIT_LINE_VERTEX( BTR(d) );
    EMIT_LINE_VERTEX( BDR(d) );
    EMIT_LINE_VERTEX( FDR(d) );
    EndPrimitive();
}

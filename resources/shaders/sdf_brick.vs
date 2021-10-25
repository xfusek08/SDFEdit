#version 460 core

smooth out vec3 pos;
smooth out vec3 col;

uniform float voxelSize;
uniform uint  voxelCount;
uniform mat4  mvp;

void main() {
    float halfEdgeLen = (float(voxelCount) * voxelSize) * 0.5;
    
    // aliases for directions
    #define UP    halfEdgeLen
    #define DOWN  -halfEdgeLen
    #define RIGHT halfEdgeLen
    #define LEFT  -halfEdgeLen
    #define FRONT halfEdgeLen
    #define BACK  -halfEdgeLen
    
    vec2 inFacePos;
    switch (gl_VertexID % 6) {
        case 0: inFacePos = vec2(LEFT,  UP);   break;
        case 1: inFacePos = vec2(LEFT,  DOWN); break;
        case 2: inFacePos = vec2(RIGHT, UP);   break;
        case 3: inFacePos = vec2(LEFT,  DOWN); break;
        case 4: inFacePos = vec2(RIGHT, DOWN); break;
        case 5: inFacePos = vec2(RIGHT, UP);   break;
        default: break;
    }
    
    // orient face vertices by face
    switch(gl_VertexID / 6) {
        case 0: pos = vec3(inFacePos.xy, FRONT); col = vec3(0,0,1); break; // front face
        case 5: pos = vec3(-inFacePos.x, inFacePos.y, BACK); col = vec3(0,0,0.5); break; // back face
        case 1: pos = vec3(inFacePos.x, UP, -inFacePos.y); col = vec3(0,1,0); break; // top face
        case 2: pos = vec3(inFacePos.x, DOWN, inFacePos.y); col = vec3(0,0.5,0); break; // bottom face
        case 3: pos = vec3(LEFT, inFacePos.y, inFacePos.x); col = vec3(0.5,0,0); break; // left face
        case 4: pos = vec3(RIGHT, inFacePos.y, -inFacePos.x); col = vec3(1,0,0); break; // right face
        default: break;
    }
    
    gl_Position = mvp * vec4(pos, 1.0f);
}

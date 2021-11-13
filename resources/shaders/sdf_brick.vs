#version 460 core

smooth out vec3 fragPos;
out vec3 fragNormal;

uniform float voxelSize;
uniform uint  voxelCount;
uniform mat4  viewProjection;

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
        case 0: // front face
            fragPos = vec3(inFacePos.xy, FRONT);
            fragNormal = vec3(0,0,1);
            break;
        case 5: // back face
            fragPos = vec3(-inFacePos.x, inFacePos.y, BACK);
            fragNormal = vec3(0,0,-1);
            break;
        case 1: // top face
            fragPos = vec3(inFacePos.x, UP, -inFacePos.y);
            fragNormal = vec3(0,1,0);
            break;
        case 2: // bottom face
            fragPos = vec3(inFacePos.x, DOWN, inFacePos.y);
            fragNormal = vec3(0,-1,0);
            break;
        case 3: // left face
            fragPos = vec3(LEFT, inFacePos.y, inFacePos.x);
            fragNormal = vec3(-1,0,0);
            break;
        case 4: // right face
            fragPos = vec3(RIGHT, inFacePos.y, -inFacePos.x);
            fragNormal = vec3(1,0,0);
            break;
        default:
            break;
    }
    // fragPos.z = 0;
    gl_Position = viewProjection * vec4(fragPos, 1.0f);
}

/**
 * Invoke this shader without any geometry:
 * glDrawArrays(GL_TRIANGLES,0,6);
 */
#version 460 core

smooth out vec3 pos;
uniform mat4  viewProjection;

void main() {
    switch(gl_VertexID) {
        case 0: pos = vec3(-1,-1,0); break; // BOTTOM LEFT
        case 1: pos = vec3(-1,1,0);  break; // TOP LEFT
        case 2: pos = vec3(1,1,0);   break; // TOP RIGHT

        case 3: pos = vec3(1,1,0);   break; // TOP RIGHT
        case 4: pos = vec3(1,-1,0);  break; // BOTTOM RIGHT
        case 5: pos = vec3(-1,-1,0); break; // BOTTOM LEFT
    }
    gl_Position = viewProjection * vec4(pos, 1);
}

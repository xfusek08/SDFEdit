/**
 * Invoke this shader without any geometry:
 * glDrawArrays(GL_TRIANGLES,0,6);
 */
#version 460 core

smooth out vec2 fragCoord;

void main() {
    switch(gl_VertexID) {
        case 0: fragCoord = vec2(-1,-1); break; // BOTTOM LEFT
        case 1: fragCoord = vec2(-1,1);  break; // TOP LEFT
        case 2: fragCoord = vec2(1,1);   break; // TOP RIGHT

        case 3: fragCoord = vec2(1,1);   break; // TOP RIGHT
        case 4: fragCoord = vec2(1,-1);  break; // BOTTOM RIGHT
        case 5: fragCoord = vec2(-1,-1); break; // BOTTOM LEFT
    }
    gl_Position = vec4(fragCoord,1,1);
}

#version 460 core

in vec2 fragCoord;
out vec4 fColor;

void main() {
    fColor = vec4(fragCoord, 0, 1);
}

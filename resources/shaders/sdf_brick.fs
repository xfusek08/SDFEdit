#version 460 core

out vec4 fColor;
in vec3 pos;
in vec3 col;

void main() {
    fColor = vec4(col, 1);
}

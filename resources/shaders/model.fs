#version 460 core

out vec4 fColor;
in vec3 fragPos;

void main() {
    fColor = vec4(fragPos.xyz, 1);
}

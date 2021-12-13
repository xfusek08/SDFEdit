#version 460 core

flat in vec4   nodeVertex;
smooth in vec3 fragPos;

out vec4 fColor;

void main() {
    // fColor = vec4(fragPos.xyz, 1);
    fColor = vec4(normalize(abs(nodeVertex.xyz)), 1);
}

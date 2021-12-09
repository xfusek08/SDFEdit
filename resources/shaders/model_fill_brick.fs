#version 460 core

layout(std430, binding = 3) buffer VertexBuffer { vec4 nodeVertices[]; };

flat   in uint nodeIndex;
smooth in vec3 fragPos;

out vec4 fColor;

void main() {
    // fColor = vec4(fragPos.xyz, 1);
    fColor = vec4(normalize(abs(nodeVertices[nodeIndex].xyz)), 1);
}

#version 460 core

layout(std430, binding = 1) buffer NodePool     { uint nodes[]; };
layout(std430, binding = 2) buffer NodeDataPool { uint nodeData[]; };
layout(std430, binding = 3) buffer VertexBuffer { vec4 nodeVertices[]; };

uniform sampler3D brickPoolTexture;

out vec4 fColor;

in vec3 fragPos;

flat in uint nodeIndex;

vec4 sampleVolume(vec3 pos)
{
    return texture(distanceVolume, pos * scale);
}

void main() {
    // fColor = vec4(fragPos.xyz, 1);
    fColor = vec4(normalize(abs(nodeVertices[nodeIndex].xyz)), 1);
}

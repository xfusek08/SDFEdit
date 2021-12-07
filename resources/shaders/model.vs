#version 460 core

#define MAX_MODELS 1000 // maximum number of modes in view

layout (location = 0) in uint vertex_nodeIndex;
layout (location = 1) in uint vertex_transformIndex;

// world translations uniform buffer block
layout (std140, binding = 4) uniform TranslationsBlock { vec4 translations[MAX_MODELS]; };

out uint nodeIndexes;

void main()
{
    nodeIndexes = vertex_nodeIndex;
    gl_Position = translations[vertex_transformIndex];
    // nodeIndex = gl_VertexID;
    // gl_Position = vec4(0,0,0,1);
}

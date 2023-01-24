#version 460 core

layout (location = 0) in vec4 brick;

out vec4 color;

uniform mat4 mvp;
uniform vec3 offset;

void main()
{
    gl_Position = vec4(offset + brick.xyz, brick.w);
    
    // color = vec4(vec3(float(gl_VertexID) / 10), 1);
    color = vec4(abs(brick.xyz) * 2, 1);
    // color = vec4(1, 1, 1, 1);
}

#version 460 core

layout (location = 0) in vec4 brick;

out vec4 color;

uniform mat4 mvp;

void main()
{
    gl_Position = brick;
    // color = vec4(vec3(float(gl_VertexID) / 10), 1);
    color = vec4(clamp(brick.xyz * 2, 0.5, 1), 1);
    // color = vec4(1, 1, 1, 1);
}

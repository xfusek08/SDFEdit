#version 460 core

layout (location = 0) in vec4 brick;
#define D_0 0.5

uniform mat4 mvp;

void main()
{
    gl_Position = vec4(brick.xyz, D_0 / (brick.w + 1));
}

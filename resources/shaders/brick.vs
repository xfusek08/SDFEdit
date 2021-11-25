#version 460 core

layout (location = 0) in vec4 brick;

void main()
{
    gl_Position = brick;
}

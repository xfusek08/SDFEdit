#version 460 core

in layout(location = 0) vec3 position;

out vec3 worldsPos;

uniform int scaleFactor;
uniform mat4 viewProjection;

void main()
{
    worldsPos = scaleFactor * position;
    gl_Position = viewProjection * vec4(worldsPos, 1.0f);
}

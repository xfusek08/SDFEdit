#version 460 core

in vec3 worldsPos;

out vec4 fColor;

void main()
{
    vec3 color = abs(worldsPos);
    float maxComp = max(max(color.x, color.y), color.z);
    float reminder = mod(maxComp, 2.0);
    
    vec3 hue = vec3(floor(reminder) * 0.3);
    color = clamp(ceil(color), 0.5, 1.0);
    fColor = vec4(color - hue, 1.0);
}

#version 460 core

smooth in vec3 fragPos;
in mat4 brickTransformMatrix;
flat in vec3  color;
flat in float shininess;


out vec4 fColor;

void main() {
    // fColor = vec4(vec3(shininess), 1);
    
    // fColor = vec4(fragPos.xyz, 1);
    
    vec3 pos = (brickTransformMatrix * vec4(fragPos, 1)).xyz;
    fColor = vec4(normalize(abs(pos)), 0.5);
}

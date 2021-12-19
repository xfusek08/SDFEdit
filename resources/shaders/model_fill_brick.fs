#version 460 core

smooth in vec3 fragPos;
in mat4 brickTransformMatrix;

out vec4 fColor;

void main() {
    // fColor = vec4(fragPos.xyz, 1);
    vec3 pos = (brickTransformMatrix * vec4(fragPos, 1)).xyz;
    fColor = vec4(normalize(abs(pos)), 1);
}

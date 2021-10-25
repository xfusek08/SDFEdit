#version 460 core

#define MAX_DISTANCE 100.0
#define MAX_STEPS 50

out vec4 fColor;
in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 cameraPosition;
uniform float voxelSize;
uniform uint  voxelCount;
uniform sampler3D distanceVolume;

struct Ray {
    vec3  position;
    vec3  direction;
};

// this pixel global pre-calculated config
float edgeLenght;
vec3  shift;
float scale;
float hitDistance;

float distToVolume(vec3 pos)
{
    vec3 sampleCoords = pos * scale;
    return texture(distanceVolume, sampleCoords).w;
}

vec3 getNormal(vec3 point) {
    float d = distToVolume(point);
    vec2 e = vec2(voxelSize * 0.1, 0);
    vec3 n = d - vec3(
        distToVolume(point - e.xyy),
        distToVolume(point - e.yxy),
        distToVolume(point - e.yyx)
    );
    return normalize(n);
}

vec4 getHitColor(vec3 pos) {
    vec3 lightPos = vec3(5, 20, 10);
    vec3 lightColor = vec3(10, 10, 10);
    vec3 ambient = vec3(1, 1, 1) / 2;
    vec3 objectColor = vec3(1, 0.4, 0.2) / 10;
    float specularStrength = 0.5;
    
    // diffuse
    vec3 normal = getNormal(pos);
    vec3 lightDir = normalize(lightPos - pos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    vec3 viewDir = normalize(cameraPosition + shift - pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    return vec4(result, 1.0);
}

void main() {
    edgeLenght  = (float(voxelCount) * voxelSize);
    shift       = vec3(edgeLenght, edgeLenght, edgeLenght) * 0.5;
    scale       = 1.0 / edgeLenght;
    hitDistance = voxelSize;
    
    Ray ray = Ray(
        fragPos + shift,
        normalize(fragPos - cameraPosition)
    );
    
    // fColor = vec4(ray.position, 1);
    // return;
    
    // ray march
    int steps = 0;
    float dist = 0;
    fColor = vec4((fragPos + shift) * scale, 0.0);
    for (steps = 0; steps < MAX_STEPS; ++steps) {
        dist = distToVolume(ray.position);
        if (dist >= MAX_DISTANCE)  {
            discard;
            // fColor = vec4((fragPos + shift) * scale, 0.1);
            break;
        }
        if (dist <= hitDistance) {
            fColor = getHitColor(ray.position);
            return;
        }
        // if (dist <= 0.5) {
            // fColor = fColor + vec4(dist / edgeLenght,dist / edgeLenght,dist / edgeLenght,0);
        // }
        ray.position = ray.position + dist * ray.direction;
    }
    discard;
}

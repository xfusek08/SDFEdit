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
uniform mat4  viewProjection;
            
struct Ray {
    vec3  position;
    vec3  direction;
};

// this pixel global pre-calculated config
float edgeLenght;
vec3  shift;
float scale;
float hitDistance;

vec4 sampleVolume(vec3 pos)
{
    return texture(distanceVolume, pos * scale);
}

// Founds lenght of ray until it exits the rendered cube
// This function was inspired by: https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
// this is reduced version of the efficient slab test from the article above
float getDistanceToEndOfBrick(Ray ray) {
    // prepare bb of current box
    vec3 maxCorner = vec3(edgeLenght, edgeLenght, edgeLenght);
    vec3 minCorner = vec3(0,0,0);
    
    vec3 inverseRayDir = 1.0 / ray.direction;
    vec3 tminv0 = (minCorner - ray.position) * inverseRayDir;
    vec3 tmaxv0 = (maxCorner - ray.position) * inverseRayDir;
    
    vec3 tmaxv = max(tminv0, tmaxv0);
    
    // return max component
    return min(tmaxv.x, min(tmaxv.y, tmaxv.z));
}

// lighting
vec4 getHitColor(vec3 pos, vec3 normal) {
    vec3 lightPos = vec3(5, 20, 10);
    vec3 lightColor = vec3(10, 10, 10);
    vec3 ambient = vec3(1, 1, 1) / 2;
    vec3 objectColor = vec3(1, 0.4, 0.2) / 10;
    float specularStrength = 0.5;
    
    // diffuse
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
    // some globals constant dependent on volume properties
    // this might be precomputed uniforms to accelerate computation
    edgeLenght  = (float(voxelCount) * voxelSize);
    shift       = vec3(edgeLenght, edgeLenght, edgeLenght)  * 0.5;
    scale       = 1.0 / edgeLenght;
    hitDistance = voxelSize * 0.5;
    
    // ray march
    Ray   ray           = Ray(fragPos + shift, normalize(fragPos - cameraPosition) );
    int   steps         = 0;
    float maxDistance   = getDistanceToEndOfBrick(ray);
    float totalDistance = 0;
    
    for (steps = 0; steps < MAX_STEPS; ++steps) {
        vec4  volumeSample = sampleVolume(ray.position);
        float actSDFValue  = volumeSample.w;
        
        // if we are near enough to the surface -> sample its color
        if (actSDFValue <= hitDistance) {
            fColor = getHitColor(ray.position, volumeSample.xyz);
            
            // https://stackoverflow.com/a/6622709
            // vec4 depth_vec = viewProjection * vec4(ray.position, 1); // viewProjection * vec4(gl_FragCoord.xyz, 1.0);
            // float depth = ((depth_vec.z / depth_vec.w) + 1.0) * 0.5;
            // gl_FragDepth = depth;
            // fColor = vec4(vec3(depth), 1);
            return;
        }
        
        // increment distance by sampled sdf value
        totalDistance += actSDFValue;
        if (totalDistance >= maxDistance) {
            // discard; // we stepped outside the cube.
            break;
        }
        
        // step the ray by sampled sdf value for next iteration
        ray.position = ray.position + actSDFValue * ray.direction;
    }
    
    // discard;
    float c = float(steps) / float(MAX_STEPS);
    fColor = vec4(1, 0, 0, c);
}

#version 460 core

// terms used in documentation of this file:
//   "octree space":
//       - A space of the octree
//       - Center of the octree is at the origin and brick verticies are shifts from the origin to all directions
//
//   "ray-marcher normalized space":
//        - unit cube with [0, 0, 0] -> [brickSize, brickSize, brickSize] corners.
//        - This is the default space in which ray marching takes place.
//        - brick has to be in octree space and then is shifted so its minimal corner is at the origin
//
//   "brickSize":
//       - An edge length of currently rasterized brick, passed to this shader in "nodeVertex.w" component

#define MAX_STEPS 50 // after N steps will marching algorithm ended event if marcher did not stepped out of the bricks volume

uniform float a;
uniform float b;
uniform float c;
uniform float d;

uniform vec3   cameraPosition; // Position of the camera will be important in the ray marching algorithm. Ray will be casted from it to thr fragment position.

smooth in vec3 fragPos;        // Fragment position in world space toward it will ray be casted from the camera
flat in vec4   nodeVertex;    // position of node/brick in octree and its edge length in w components
flat in float  voxelHalfSize; // size of half of one voxel
in mat4        brickToOctreeSpaceMatrix;

out vec4 fColor; // calculated fragment color

// // generally useful pre-computed data from previous stages to avoid pooling them from global memory
// // We could say that those are common values for whole bricks
// in float voxelHalfSize;   // This can be precomputed for whole brick in vertex shader.
// in mat4  brickToOriginTransformMatrix; // transform matrix which moves


// brick atlas related stuff
#ifndef BRICK_SIDE_LENGTH
    #define BRICK_SIDE_LENGTH 8  // lets have NxNxN bricks
#endif
uniform sampler3D brickAtlas;
uniform float     brickAtlasScale; // This value scales down a position in ray-marcher normalized space to the position inside single brick in brick atlas, this is precomputed for whole dispatch because it is dependent only on dimensions of the atlas
flat in vec3      brickAtlasShift; // vector shifting scaled position inside the brick volume to correct position inside the brick atlas, this is precomputed per brick in vertex shader
flat in float     brickInvertedSize;

float sampleVolume(vec3 pos) {
    return texture(
        brickAtlas,
        // d * (brickInvertedSize * pos * ((8.0) / 300.0) + brickAtlasShift + vec3((1.0) / (300.0))) + vec3(a,b,c)
        // pos * ((8.0 * d) / 300.0) + brickAtlasShift + vec3((1.0 + a) / (300.0))
        pos * ((8.0) / 300.0) + brickAtlasShift + vec3((1.0) / (300.0))
    ).r;
    // return texture(
    //     brickAtlas,
    //     brickInvertedSize * brickAtlasScale * pos + brickAtlasShift
    // ).r;
}


// ray marching algorithm related stuff
struct Ray {                  // the helper data structure to where store ray marcher state
    vec3  origin;
    vec3  direction;
    float dist;
};

// lighting related stuff
vec3 getNormal(vec3 point, float currentDistance) {
    vec2 e = vec2(0.05, 0);
    vec3 n = vec3(
        sampleVolume(point + e.xyy),
        sampleVolume(point + e.yxy),
        sampleVolume(point + e.yyx)
    ) - currentDistance;
    return normalize(n);
}
vec4 getHitColor(vec3 pos, vec3 normal, vec3 camPos) {
    vec3 lightColor = vec3(1, 1, 1);
    vec3 ambient = vec3(1, 1, 1) * 0.3;
    vec3 objectColor = vec3(1, 0.4, 0.2);
    float specularStrength = 0.5;
    
    // diffuse
    vec3 lightDir = normalize(vec3(1, 1, 1));
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    vec3 viewDir = normalize(camPos - pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    return vec4(result, 1.0);
}

// Founds length of ray until it exits the rendered cube
// This function was inspired by:
//    https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
//    - implemented reduced version of the efficient slab test algorithm
float getDistanceToEndOfBrick(vec3 position, vec3 direction) {
    
    // prepare bb of current box
    vec3 maxCorner = vec3(1,1,1);
    vec3 minCorner = vec3(0,0,0);
    
    vec3 inverseRayDir = 1.0 / direction;
    vec3 tMinV0 = (minCorner - position) * inverseRayDir;
    vec3 tMaxV0 = (maxCorner - position) * inverseRayDir;
    
    vec3 tMaxV = max(tMinV0, tMaxV0);
    
    return min(tMaxV.x, min(tMaxV.y, tMaxV.z));
}

void rayMarch(Ray ray, vec3 camPos) {
    float maxDistance = getDistanceToEndOfBrick(ray.origin, ray.direction);
    
    uint steps = 0; // counted loop for debug evaluation
    for (steps = 0; steps < MAX_STEPS; ++steps) {
        if (ray.dist >= maxDistance) { // check if we stepped outside the brick.
            discard;
            break;
        }
        
        vec3  actPosition  = (ray.dist * ray.direction) + ray.origin;
        float distToVolume = sampleVolume(actPosition);
        
        if (distToVolume <= 0.001) {
            fColor = getHitColor(actPosition, getNormal(actPosition, distToVolume), camPos);
            return;
        }
        
        ray.dist += distToVolume; // make step into volume
    }
    
    // discard;
    float c = float(steps) / float(MAX_STEPS);
    fColor = vec4(1, 0, 0, c);
}

void showAtlasSamples(Ray ray) {
    fColor = vec4(vec3(sampleVolume(ray.origin)), 1);
}

void showVoxels(Ray ray) {
    float maxDistance = getDistanceToEndOfBrick(ray.origin, ray.direction);
    vec3 pos = ray.origin;
    // pos = floor((pos + vec3(a,b,c)) * (8.0 * d));
    fColor = vec4(pos, 1);
    // fColor = vec4(texture(brickAtlas, pos).rrr, 1);
    // fColor = vec4(texelFetch(brickAtlas, ivec3(pos), 0).rrr, 1);
}

void main() {
    
    // 1. create ray in ray-marcher space
    // 1.1 transform fragment position and camera position of the brick to octree space
    vec3 _fragPos        = (brickToOctreeSpaceMatrix * vec4(fragPos, 1)).xyz;
    vec3 _cameraPosition = (brickToOctreeSpaceMatrix * vec4(cameraPosition, 1)).xyz;
    
    // 1.2 shift fragment position and camera position of the brick to ray-marcher space
    _fragPos        += nodeVertex.www * 0.5 - nodeVertex.xyz;
    _cameraPosition += nodeVertex.www * 0.5 - nodeVertex.xyz; // TODO: shifted camera position can be computed per brick in vertex shader
    
    _fragPos        *= brickInvertedSize;
    _cameraPosition *= brickInvertedSize;
    
    Ray ray = Ray(_fragPos, normalize(_fragPos - _cameraPosition), 0);
    
    // fColor = vec4(ray.origin, 1);
    // fColor = vec4(vec3(getDistanceToEndOfBrick(ray.origin, ray.direction)), 1);
    // fColor = vec4(vec3(sampleVolume(ray.origin)), 1);
    rayMarch(ray, _cameraPosition);
    // showAtlasSamples(ray);
    // showVoxels(ray);
}

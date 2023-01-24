#version 460 core

// terms used in documentation of this file:
//   "octree space":
//       - A space of the octree
//       - Center of the octree is at the origin and brick vertices are shifts from the origin to all directions
//
//   "ray-marcher normalized space":
//        - unit cube with [0, 0, 0] -> [1, 1, 1] corners.
//        - This is the default space in which ray marching takes place.
//        - brick has to be in octree space and then is shifted so its minimal corner is at the origin

#define MAX_STEPS     50   // After N steps will marching algorithm ended event if marcher did not stepped out of the bricks volume
#define NORMAL_OFFSET 0.06 // Normal difference distance to properly sample brick volume - optimized for 8x8x8 brick (without border)
#define HIT_DISTANCE  0.01 // Distance from surface considered as hit.

uniform vec3 cameraPosition;
uniform mat4 viewProjection;

// inputs
smooth in vec3 fragPos;       // Fragment position in world space toward it will ray be casted from the camera
in mat4 brickTransformMatrix; // Matrix transforming position of fragment from world space to local ray-marcher space
in vec3 brickCameraPosition;  // Position of camera prepared in ray-marcher space
flat in vec3  color;
flat in float shininess;

// outputs
out vec4 fColor; // calculated fragment color

// brick atlas sampling related stuff
uniform sampler3D brickAtlas;
uniform float     brickAtlasScale; // This value scales down a position in ray-marcher normalized space to the position inside single brick in brick atlas, this is precomputed for whole dispatch because it is dependent only on dimensions of the atlas
flat in vec3      brickAtlasShift; // vector shifting scaled position inside the brick volume to correct position inside the brick atlas, this is precomputed per brick in vertex shader
float sampleVolume(vec3 pos) {
    return texture(brickAtlas, pos * brickAtlasScale + brickAtlasShift).r;
}

// ray marching algorithm related stuff
struct Ray {                  // the helper data structure to where store ray marcher state
    vec3  origin;
    vec3  direction;
    float dist;
};

// Compute normal of the point it is computed within a
// computation is done in ray-marching space
vec3 getNormal(vec3 point, float currentDistance) {
    vec2 e = vec2(NORMAL_OFFSET, 0);
    vec3 n = vec3(
        sampleVolume(point + e.xyy),
        sampleVolume(point + e.yxy),
        sampleVolume(point + e.yyx)
    ) - currentDistance;
    return normalize(n);
}

vec3 toLocal(vec3 pos) {
    return (brickTransformMatrix * vec4(pos, 1)).xyz;
}

// Computing basic Phong lighting
vec4 getHitColor(vec3 pos, vec3 normal) {
    vec3  lightPos         = toLocal(vec3(100, 100, 100));
    vec3  lightColor       = vec3(1, 1, 1);
    vec3  ambient          = vec3(1, 1, 1) * 0.25;
    vec3  objectColor      = color;
    float specularStrength = shininess;
    
    // diffuse
    vec3  lightDir = normalize(lightPos);
    float diff     = max(dot(normal, lightDir), 0.0);
    vec3  diffuse  = diff * lightColor;
    
    // specular
    vec3  viewDir    = normalize(toLocal(cameraPosition) - pos);
    vec3  reflectDir = reflect(-lightDir, normal);
    float spec       = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3  specular   = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    return vec4(result, 1.0);
}

// Founds length of ray until it exits the rendered cube
// Computed in ray-marcher space
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

void main() {
    vec3 brickFragmentPos = toLocal(fragPos);
    Ray ray = Ray(brickFragmentPos, normalize(brickFragmentPos - brickCameraPosition), 0);
    
    // ray march:
    
    float maxDistance = getDistanceToEndOfBrick(ray.origin, ray.direction);
    
    uint steps = 0; // counted loop for debug evaluation
    for (steps = 0; steps < MAX_STEPS; ++steps) {
        if (ray.dist >= maxDistance) { // check if we stepped outside the brick.
            discard;
        }
        
        vec3  actPosition  = (ray.dist * ray.direction) + ray.origin;
        float distToVolume = sampleVolume(actPosition);
        
        if (distToVolume <= HIT_DISTANCE) {
            fColor = getHitColor(actPosition, getNormal(actPosition, distToVolume));
            
            // Depth buffer value correction
            // see: https://stackoverflow.com/questions/53650693/opengl-impostor-sphere-problem-when-calculating-the-depth-value
            vec4 c = viewProjection * inverse(brickTransformMatrix) * vec4(actPosition, 1);
            float ndcDepth = c.z / c.w;
            gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;
            
            return;
        }
        ray.dist += distToVolume; // make step into volume
    }
    discard;
}

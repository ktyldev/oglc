#version 430

#include sphere.glsl

layout(local_size_x = 1, local_size_y = 1) in;                  // size of local work group - 1 pixel

// TODO: do i actually need explicit location descriptors?
layout (location = 1)   uniform vec4 _t;

layout (location = 2)   uniform vec3 _w;                        // view space axes
layout (location = 3)   uniform vec3 _u;
layout (location = 4)   uniform vec3 _v;

layout (location = 5)   uniform mat4 _cameraInverseProjection;
layout (location = 6)   uniform vec3 _camh;
layout (location = 7)   uniform vec3 _camv;
layout (location = 8)   uniform vec3 _camll;
layout (location = 9)   uniform vec3 _cpos;
layout (location = 10)  uniform vec3 _tpos;                     // target

// 253 is the maximum?? TODO: use uniform buffer objects
const int SPHERES = 250; 

layout (location = 12)  uniform int _activeSpheres;
layout (location = 13)  uniform Sphere _spheres[SPHERES];

uniform vec4 _seed;


layout(rgba32f, binding = 0) uniform image2D img_output;        // rgba32f defines internal format, image2d for random write to output texture
layout(binding=1) uniform sampler2D _noise;                     // noise texture

const float INF = 1000.0;
const float PI = 3.14159;

struct Ray 
{
    vec3 origin;
    vec3 direction;
    vec3 energy;
};
Ray createRay(vec3 origin, vec3 direction)
{
    Ray ray;

    ray.origin = origin;
    ray.direction = direction;
    ray.energy = vec3(1.0,1.0,1.0);

    return ray;
}

struct RayHit
{
    vec3 position;
    float distance;
    vec3 normal;
    vec3 albedo;
};
RayHit createRayHit()
{
    RayHit hit;

    hit.position = vec3(0.0,0.0,0.0);
    hit.distance = INF;
    hit.normal = vec3(0.0,0.0,0.0);
    hit.albedo = vec3(0.0,0.0,0.0);

    return hit;
}

void intersectSphere(Ray ray, inout RayHit bestHit, Sphere sphere)
{
    vec3 c = sphere.cr.xyz;
    float r = sphere.cr.w;

    vec3 d = ray.origin-c;
    float p1 = -dot(ray.direction,d);
    float p2sqr = p1*p1-dot(d,d)+r*r;

    if (p2sqr < 0) return;

    float p2 = sqrt(p2sqr);
    float t = p1-p2 > 0 ? p1-p2 : p1+p2;
    if (t > 0 && t < bestHit.distance)
    {
        bestHit.distance = t;
        bestHit.position = ray.origin + t * ray.direction;
        bestHit.normal = normalize(bestHit.position-c);
        bestHit.albedo = sphere.albedo;
    }
}

void intersectPlane(Ray ray, inout RayHit bestHit, vec3 p, vec3 normal)
{
    //normal = vec3(0.0,0.0,1.0);
    float denom = dot(normal, ray.direction);

    if (abs(denom) > 0.0001)
    {
        float t = dot(p-ray.origin, normal) / denom;
        if (t >= 0 && t < bestHit.distance)
        {
            bestHit.distance = t;
            bestHit.position = ray.origin + t*ray.direction;
            bestHit.normal = normal;
            bestHit.albedo = vec3(1.0,1.0,1.0);
        }
    }
}

Ray createCameraRay(vec2 uv)
{
    // transform -1..1 -> 0..1
    uv = uv*0.5+0.5;
    //uv.x=1-uv.x;

    vec3 target = vec3(0,0,0);

    vec3 dir;
    dir = uv.x*_camh + uv.y*_camv;
    dir = _camll + uv.x*_camh + uv.y*_camv;
    dir = normalize(dir);

    Ray ray = createRay(_cpos, dir);

    return ray;
};

RayHit trace(Ray ray)
{
    RayHit hit = createRayHit();

    // TODO: intersect something other than spheres

    Sphere s;
    s.cr = vec4(0.0,0.0,0.0,2.0);
    s.albedo = vec3(1.0,0.0,0.0);

    intersectPlane(ray, hit, vec3(0.0,-1.5,0.0),vec3(0.0,1.0,0.0));

    for (int i = 0; i < _activeSpheres; i++)
    {
        intersectSphere(ray, hit, _spheres[i]);
    }

    //intersectSphere(ray, hit, s);

    return hit;
}

vec2 pixelUv() 
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    ivec2 dims = imageSize(img_output);

    vec2 uv;
    uv.x = (float(pixelCoords.x * 2 - dims.x) / dims.x) * dims.x/dims.y;       // account for aspect ratio
    uv.y = (float(pixelCoords.y * 2 - dims.y) / dims.y);

    return uv;
}

vec4 sampleNoise()
{
    return texture(_noise, pixelUv());
}

float random(vec2 st)
{
    //st += gl_GlobalInvocationID.xy;
    //st += _seed.xy;
    //st += _seed.zw;
    //normalize(st);

    vec2 nuv = texture(_noise, st.xy).xy;

    return fract(sin(dot(nuv,vec2(12.9898,78.233)))*43758.5453123);
}

float sdot(vec3 x, vec3 y, float f = 1.0)
{
    return clamp(dot(x,y)*f,0.0,1.0);
}

mat3 getTangentSpace(vec3 normal)
{
    vec3 helper = abs(normal.x) > 0.99 
        ? vec3(1.0,0.0,0.0) 
        : vec3(0.0,0.0,1.0);

    vec3 tangent = normalize(cross(normal, helper));
    vec3 binormal = normalize(cross(normal, tangent));

    return mat3(tangent, binormal, normal);
}

vec3 sampleHemisphere(vec3 normal)
{
    vec4 noise = sampleNoise();

    float cosTheta = random(normalize(normal.xy+noise.xy));
    float sinTheta = sqrt(max(0.0,1.0-cosTheta*cosTheta));

    float phi = 2.0*PI*random(normalize(normal.yz+noise.xw));
    vec3 tangentSpaceDir = vec3(cos(phi)*sinTheta, sin(phi)*sinTheta, cosTheta);

    // convert direction from tangent space to world space
    mat3 ts = getTangentSpace(normal);
    return ts * tangentSpaceDir;
}

vec3 scatterLambert(inout Ray ray, RayHit hit)
{
    ray.origin = hit.position + hit.normal*0.001;
    ray.direction = sampleHemisphere(hit.normal);
    ray.energy *= 2.0 * hit.albedo * sdot(hit.normal, ray.direction);

    return vec3(0.0);
}

vec3 shade(inout Ray ray, RayHit hit)
{
    if (hit.distance < INF)
    {
        return scatterLambert(ray, hit);
    }

    //ray.energy = vec3(0.0);

    // sky color
    return vec3(0.68,0.85,0.9);
}

void main()
{
    // base pixel colour for the image
    vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
    // get index in global work group ie xy position
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv = pixelUv();

    int samples = 2;
    int bounces = 2;

    for (int i = 0; i < samples; i++) 
    {
        // create a ray from the uv
        Ray ray = createCameraRay(uv);

        // trace the rays path around the scene
        for (int j = 0; j < bounces; j++)
        {
            RayHit hit = trace(ray);

            pixel.xyz += ray.energy * shade(ray, hit);

            if (length(ray.energy) < 0.001) break;
        }
    }

    pixel.xyz /= samples;

    // TODO: write depth to texture
    //float depth = hit.distance/INF;
    //pixel = vec4(hit.albedo,1.0);
    //pixel *= (1.0-depth);

    //pixel = texture(_noise, uv);

    // output to a specific pixel in the image
    imageStore(img_output, pixel_coords, pixel);
}

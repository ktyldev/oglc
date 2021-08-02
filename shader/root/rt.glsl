#version 430

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

layout(local_size_x = 1, local_size_y = 1) in;                  // size of local work group - 1 pixel
layout(rgba32f, binding = 0) uniform image2D img_output;        // rgba32f defines internal format, image2d for random write to output texture

const float INF = 30.0;

#include sphere.glsl

struct Ray 
{
    vec3 origin;
    vec3 direction;
};

struct RayHit
{
    vec3 position;
    float distance;
    vec3 normal;
    vec3 albedo;
};

void intersectSphere(Ray ray, inout RayHit bestHit, Sphere sphere)
{
    vec3 d = ray.origin-sphere.center;
    float p1 = -dot(ray.direction,d);
    float p2sqr = p1*p1-dot(d,d)+sphere.radius*sphere.radius;

    if (p2sqr < 0) return;

    float p2 = sqrt(p2sqr);
    float t = p1-p2 > 0 ? p1-p2 : p1+p2;
    if (t > 0 && t < bestHit.distance)
    {
        bestHit.distance = t;
        bestHit.position = ray.origin + t*ray.direction;
        bestHit.normal = normalize(bestHit.position-sphere.center);
        bestHit.albedo = sphere.albedo;
    }
}

Ray createCameraRay(vec2 uv)
{
    // transform -1..1 -> 0..1
    uv = uv*0.5+0.5;
    //uv.x=1-uv.x;

    vec3 target = vec3(0,0,0);

    // transform camera origin to world space
    // TODO: c2w matrix!! for now we just assume the camera is at the origin
    // float3 origin = mul(_CameraToWorld, float4(0.0,0.0,0.0,1.0)).xyz;

    // TODO: offset from centre of the lens for depth of field
    // float2 rd = _CameraLensRadius * randomInUnitDisk();
    // float3 offset = _CameraU * rd.x + _CameraV * rd.y;

    // invert perspective projection of view space position
    //vec3 dir = mul(_cameraInverseProjection, float4(uv, 0.0, 1.0)).xyz;

    // TODO: transform direction from camera to world space (move camera around!)

    vec3 dir;
    dir = uv.x*_camh + uv.y*_camv;
    dir = _camll + uv.x*_camh + uv.y*_camv;
    dir = normalize(dir);

    float max_x = 5.0;
    float max_y = 5.0;

    Ray ray;
    ray.origin = _cpos;
    ray.direction = dir;

    return ray;
};

void main()
{
    // base pixel colour for the image
    vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);
    // get index in global work group ie xy position
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    // set up ray based on pixel position, project it forward with an orthographic projection
    ivec2 dims = imageSize(img_output);                                         // fetch image dimensions
    vec2 uv;
    uv.x = (float(pixel_coords.x * 2 - dims.x) / dims.x) * dims.x/dims.y;       // account for aspect ratio
    uv.y = (float(pixel_coords.y * 2 - dims.y) / dims.y);

    Ray ray = createCameraRay(uv);

    RayHit hit;
    hit.position = vec3(0.0,0.0,0.0);
    hit.distance = INF;
    hit.normal = vec3(0.0,0.0,0.0);
    hit.albedo = vec3(0.0,0.0,0.0);

    vec3 spheresCenter = vec3(0.0,0.0,0.0);

    float t = _t.x;
    Sphere s1;
    s1.center = spheresCenter+vec3(sin(t),0.0,cos(t))*2.5;
    s1.radius = 2.0;
    s1.albedo = vec3(1.0,0.0,0.0);

    t+=3.1415/1.5;
    Sphere s2;
    s2.center = spheresCenter+vec3(sin(t),0.0,cos(t))*2.5;
    s2.radius = 2.0;
    s2.albedo = vec3(0.0,1.0,0.0);

    t+=3.1415/1.5;
    Sphere s3;
    s3.center = spheresCenter+vec3(sin(t),0.0,cos(t))*2.5;
    s3.radius = 2.0;
    s3.albedo = vec3(0.0,0.0,1.0);

    // ray-sphere intersection
    intersectSphere(ray, hit, s1);
    intersectSphere(ray, hit, s2);
    intersectSphere(ray, hit, s3);

    // TODO: write depth to texture
    float depth = hit.distance/INF;

    pixel = vec4(hit.albedo,1.0);
    pixel *= (1.0-depth);

    // output to a specific pixel in the image
    imageStore(img_output, pixel_coords, pixel);
}

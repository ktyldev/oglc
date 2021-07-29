#version 430

layout (location = 1) uniform vec4 t;

layout(local_size_x = 1, local_size_y = 1) in;              // size of local work group - 1 pixel
layout(rgba32f, binding = 0) uniform image2D img_output;    // rgba32f defines internal format, image2d for random write to output texture

const float INF = 1000000.0f;

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
    }
}

Ray createCameraRay(vec2 uv)
{
    // transform -1..1 -> 0..1
    //uv = uv*0.5+0.5;
    //uv.x=1-uv.x;

    // transform camera origin to world space
    // TODO: c2w matrix!! for now we just assume the camera is at the origin
    // float3 origin = mul(_CameraToWorld, float4(0.0,0.0,0.0,1.0)).xyz;

    // TODO: offset from centre of the lens for depth of field
    // float2 rd = _CameraLensRadius * randomInUnitDisk();
    // float3 offset = _CameraU * rd.x + _CameraV * rd.y;

    // ...

    float max_x = 5.0;
    float max_y = 5.0;

    Ray ray;
    ray.origin = vec3(uv.x * max_x, uv.y * max_y, 0.0);
    ray.direction = vec3(0.0,0.0,1.0);                                          // ortho forwards

    return ray;
}

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

    Sphere sphere;
    sphere.center = vec3(0.0,0.0,10.0);
    sphere.radius = 3.0+t.y;

    // ray-sphere intersection
    intersectSphere(ray, hit, sphere);

    if (hit.distance < INF)
    {
        pixel = vec4(t.y,1.0-t.y,1.0,1.0);
    }

    // output to a specific pixel in the image
    imageStore(img_output, pixel_coords, pixel);
}

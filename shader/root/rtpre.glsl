#version 430

// writes first hit information into buffers to accelerate later passes

// local work group
layout(local_size_x = 1, local_size_y = 1) in;

layout (rgba32f, binding = 2) uniform image2D g0_output;

#include func.glsl
#include constants.glsl
#include time.glsl
#include sphere.glsl
#include ray.glsl
#include intersect.glsl
#include random.glsl
#include camera.glsl
#include image.glsl
#include scene.glsl

void main()
{
    // x    normal.x
    // y    normal.y
    // z    normal.z
    // w    depth

    vec4 pixel= vec4(0);
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(g0_output);
    vec2 uv = pixelUv(pixelCoords, dims);

    Ray ray = createCameraRay(uv);
    RayHit hit = trace(ray);

    //pixel.xyz = hit.normal;

    // TODO: non-linear depth
    float n = 1;
    float f = INF;
    float z = hit.distance;

    float depth = (1.0/z-1.0/n)/(1.0/n-1.0/f);

    pixel.x = z/f;

    imageStore(g0_output, pixelCoords, pixel);
}

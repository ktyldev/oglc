#version 430

// writes first hit information into buffers to accelerate later passes

// local work group
layout(local_size_x = 1, local_size_y = 1) in;

layout (rgba32f, binding = 2) writeonly uniform image2D g0_output;
layout (rgba32f, binding = 3) writeonly uniform image2D g1_output;

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
#include depth.glsl

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

    float depth = getLogarithmicDepth(hit.distance);

    // pack normal into texture
    pixel.xyz = hit.normal*0.5+0.5;
    pixel.w = depth;

    imageStore(g0_output, pixelCoords, pixel);

    pixel.xyz = hit.albedo;
    pixel.w = 0;
    imageStore(g1_output, pixelCoords, pixel); 
}

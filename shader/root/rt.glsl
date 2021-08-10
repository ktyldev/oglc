#version 430

layout(local_size_x = 1, local_size_y = 1) in;                  // size of local work group - 1 pixel

// final output
layout(rgba32f, binding = 0) uniform image2D img_output;        // rgba32f defines internal format, image2d for random write to output texture

// TODO: some of these depend on each other!! need be be in this order for now c:
#include func.glsl
#include constants.glsl
#include time.glsl
#include sphere.glsl
#include ray.glsl
#include intersect.glsl
#include random.glsl
#include camera.glsl
#include image.glsl
// scene.glsl includes scene trace function
#include scene.glsl     
#include lighting.glsl

vec3 shade(inout Ray ray, RayHit hit)
{
    if (hit.distance < INF)
    {
        return scatterLambert(ray, hit);
    }

    // sky color
    return vec3(0.68,0.85,0.9);
}

void main()
{
    // base pixel colour for the image
    vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

    vec2 uv = pixelUv();

    int samples = 1;
    int bounces = 4;

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

    // output to a specific pixel in the image
    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), pixel);
}

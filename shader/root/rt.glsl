#version 430

layout(local_size_x = 1, local_size_y = 1) in;                  // size of local work group - 1 pixel

// gbuffer?
layout(rgba32f, binding = 2) readonly uniform image2D _g0;

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

    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(img_output);

    vec2 uv = pixelUv(pixelCoords, dims);

    for (int i = 0; i < SAMPLES; i++) 
    {
        // create a ray from the uv
        Ray ray = createCameraRay(uv);

        // trace the rays path around the scene
        for (int j = 0; j < BOUNCES; j++)
        {
            RayHit hit = trace(ray);

            pixel.xyz += ray.energy * shade(ray, hit);

            if (length(ray.energy) < 0.001) break;
        }
    }
    pixel.xyz /= SAMPLES;

    vec4 d = imageLoad(_g0, ivec2(gl_GlobalInvocationID.xy));
    float depth = d.x;

    pixel.xyz = mix(pixel.xyz, vec3(0), depth);

    //pixel.a = 1.0;

    //pixel.xyz = texture(_g0, uv).xyz;

    // output to a specific pixel in the image
    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), pixel);
}

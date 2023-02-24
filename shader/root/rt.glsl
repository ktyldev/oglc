#version 430

layout(local_size_x = 1, local_size_y = 1) in;                  // size of local work group - 1 pixel

// gbuffer?
layout(rgba32f, binding = 2) readonly uniform image2D _g0;
layout(rgba32f, binding = 3) readonly uniform image2D _g1;

// final output
layout(rgba32f, binding = 0) uniform image2D img_output;        // rgba32f defines internal format, image2d for random write to output texture

uniform vec3 _skyColor = vec3(0.68,0.85,0.9);

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
#include depth.glsl

vec3 shade(inout Ray ray, RayHit hit)
{
    if (hit.distance < INF)
    {
        switch (hit.material)
        {
        case MAT_SKY: break;
        case MAT_LAMBERT: 
            scatterLambert(ray, hit);
            break;
        case MAT_CHROME: 
            return scatterMetal(ray, hit);
            break;
        }
    }

    // sky color
    return _skyColor;
}

void main()
{
    // base pixel colour for the image
    vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(img_output);

    vec2 uv = pixelUv(pixelCoords, dims);

    vec4 g[2];

    // load data from first pass
    g[0] = imageLoad(_g0, ivec2(gl_GlobalInvocationID.xy));
    float depth = g[0].w;
    vec3 normal = g[0].xyz*2.0-1.0; // unpack normal packaged into texture

    g[1] = imageLoad(_g1, ivec2(gl_GlobalInvocationID.xy));
    vec3 albedo = g[1].xyz;

    // create a ray from the uv
    Ray ray = createCameraRay(uv);
    RayHit firstHit;
    firstHit.position = ray.origin+ray.direction*depth;
    firstHit.distance = depth;
    firstHit.normal = normal;
    firstHit.albedo = albedo;

    int sky = depth >= INF ? 1 : 0;
    int bounces = (1-sky) * BOUNCES; 
    pixel.xyz = mix(pixel.xyz, _skyColor, sky);

    // trace the ray's path around the scene
    for (int j = 0; j < bounces; j++)
    {
        RayHit hit = trace(ray);

        depth = getLogarithmicDepth(ray.distance);

        pixel.xyz += ray.energy * shade(ray, hit);

        if (length(ray.energy) < 0.001) break;
    }

    pixel.xyz = mix(pixel.xyz, vec3(1.0), depth);

    // output to a specific pixel in the image
    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), pixel);
}

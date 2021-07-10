#version 430

layout (location = 1) uniform float t;

// size of local work group - 1 pixel
layout(local_size_x = 1, local_size_y = 1) in;

// rgba32f defines internal format, image2d for random write to output texture
layout(rgba32f, binding = 0) uniform image2D img_output;


void main()
{
    // base pixel colour for the image
    vec4 pixel = vec4(1.0, t, 0.0, 1.0);
    // get index in global work group ie xy position
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    //

    // output to a specific pixel in the image
    imageStore(img_output, pixel_coords, pixel);
}

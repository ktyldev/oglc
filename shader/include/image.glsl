vec2 pixelUv() 
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    ivec2 dims = imageSize(img_output);

    vec2 uv;
    uv.x = (float(pixelCoords.x * 2 - dims.x) / dims.x) * dims.x/dims.y;       // account for aspect ratio
    uv.y = (float(pixelCoords.y * 2 - dims.y) / dims.y);

    return uv;
}

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
    // TODO: make independent of this image uniform 
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(img_output);

    vec2 uv = pixelUv(pixelCoords, dims);
    uv += _seed.xy;

    vec4 noise = sampleNoise(uv);;

    float cosTheta = random(normalize(normal.xy+noise.xy));
    float sinTheta = sqrt(max(0.0,1.0-cosTheta*cosTheta));

    float phi = 2.0*PI*random(normalize(normal.yz+noise.xw));
    vec3 tangentSpaceDir = vec3(cos(phi)*sinTheta, sin(phi)*sinTheta, cosTheta);

    // convert direction from tangent space to world space
    mat3 ts = getTangentSpace(normal);
    return ts * tangentSpaceDir;
}


vec3 scatterMetal(inout Ray ray, RayHit hit)
{
    ray.origin = hit.position + hit.normal*0.001;
    ray.direction = reflect(ray.direction,hit.normal);
    ray.energy *= 0.5;

    return vec3(0.0);
}

vec3 scatterLambert(inout Ray ray, RayHit hit)
{
    ray.origin = hit.position + hit.normal*0.001;
    ray.direction = sampleHemisphere(hit.normal);
    ray.energy *= hit.albedo * sdot(hit.normal, ray.direction);

    return vec3(0.0);
}

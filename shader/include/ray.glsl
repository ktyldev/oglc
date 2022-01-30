struct Ray
{
    vec3 origin;
    vec3 direction;
    vec3 energy;
};
Ray createRay(vec3 origin, vec3 direction)
{
    Ray ray;

    ray.origin = origin;
    ray.direction = direction;
    ray.energy = vec3(1.0,1.0,1.0);

    return ray;
}

struct RayHit
{
    vec3 position;
    float distance;
    vec3 normal;
    vec3 albedo;
    int material;
};
RayHit createRayHit()
{
    RayHit hit;

    hit.position = vec3(0.0,0.0,0.0);
    // TODO: this might not be defined
    hit.distance = INF;
    hit.normal = vec3(0.0,0.0,0.0);
    hit.albedo = vec3(0.0,0.0,0.0);

    return hit;
}

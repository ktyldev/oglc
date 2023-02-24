void intersectSphere(Ray ray, inout RayHit bestHit, Sphere sphere)
{
    vec3 c = sphere.cr.xyz;
    float r = sphere.cr.w;

    vec3 d = ray.origin-c;
    float p1 = -dot(ray.direction,d);
    float p2sqr = p1*p1-dot(d,d)+r*r;

    if (p2sqr < 0) return;

    float p2 = sqrt(p2sqr);
    float t = p1-p2 > 0 ? p1-p2 : p1+p2;
    if (t > 0 && t < bestHit.distance)
    {
        bestHit.distance = t;
        bestHit.position = ray.origin + t * ray.direction;
        bestHit.normal = normalize(bestHit.position-c);
        bestHit.albedo = sphere.albedo;
        bestHit.material = sphere.material;
    }
}

void intersectPlane(Ray ray, inout RayHit bestHit, vec3 p, vec3 normal)
{
    float denom = dot(normal, ray.direction);

    if (abs(denom) > 0.0001)
    {
        float t = dot(p-ray.origin, normal) / denom;
        if (t >= 0 && t < bestHit.distance)
        {
            bestHit.distance = t;
            bestHit.position = ray.origin + t*ray.direction;
            bestHit.normal = normal;
            bestHit.albedo = vec3(1.0,.4,.4);
            bestHit.material = MAT_LAMBERT;
        }
    }
}

RayHit trace(Ray ray)
{
    RayHit hit = createRayHit();

    // floor
    intersectPlane(ray, hit, vec3(0.0,-1.5,0.0),vec3(0.0,1.0,0.0));

    // spheres
    for (int i = 0; i < _activeSpheres; i++)
    {
        intersectSphere(ray, hit, _spheres[i]);
    }

    return hit;
}

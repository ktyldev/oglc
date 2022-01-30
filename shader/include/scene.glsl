RayHit trace(Ray ray)
{
    RayHit hit = createRayHit();

    Sphere s;
    s.cr = vec4(0.0,0.0,0.0,5.0);
    s.albedo = vec3(1.0,0.0,0.0);
    s.material = MAT_CHROME;
    intersectSphere(ray, hit, s);

    intersectPlane(ray, hit, vec3(0.0,-1.5,0.0),vec3(0.0,1.0,0.0));

    for (int i = 0; i < _activeSpheres; i++)
    {
        intersectSphere(ray, hit, _spheres[i]);
    }

    return hit;
}

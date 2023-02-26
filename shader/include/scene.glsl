RayHit trace(inout Ray ray)
{
    RayHit hit = createRayHit();

    // floor
    intersectPlane(ray, hit, vec3(0.0,-1.5,0.0),vec3(0.0,1.0,0.0));

    // spheres
    for (int i = 0; i < _activeSpheres; i++)
    {
        intersectSphere(ray, hit, _spheres[i]);
    }

    int sphereCount = 10;
    for (int i = 0; i < sphereCount; i++)
    {
        Sphere s;
        float a = i/float(sphereCount)*2.0*PI;
        float d = 17.0 + cos((1.3+a)*3.0) * 3.0;
        float r = 4.0 + sin(a*3.0)*2.0;
        s.cr = vec4(sin(a)*d,2.0*r+cos(a*5.0),cos(a)*d, r);
        s.albedo = vec3(.2);
        s.material = i % 3 == 0 ? MAT_CHROME : MAT_LAMBERT;

        intersectSphere(ray, hit, s);
    }

    sphereCount = 3;
    for (int i = 0; i < sphereCount; i++)
    {
        Sphere s;
        float a = i/float(sphereCount)*2.0*PI;
        float d = 5.0 + cos((5.34+a)*5.0) * 3.0;
        float r = 3.0 + sin(a*2.0)*1.5;
        s.cr = vec4(sin(a)*d,4.0*r+cos(a*5.0),cos(a)*d, r);
        s.albedo = vec3(.2);
        s.material = i % 3 == 0 ? MAT_CHROME : MAT_LAMBERT;

        intersectSphere(ray, hit, s);
    }

    ray.distance += hit.distance * float(hit.distance < INF);

    return hit;
}

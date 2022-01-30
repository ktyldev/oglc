struct Sphere
{
    // (c.x,c.y,c.z,r)
    vec4 cr;
    vec3 albedo;
    int material;
};

// TODO: use uniform buffer objects
const int SPHERES = 167;
uniform int _activeSpheres;
layout (location = 1)  uniform Sphere _spheres[SPHERES];

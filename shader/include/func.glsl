float sdot(vec3 x, vec3 y, float f = 1.0)
{
    return clamp(dot(x,y)*f,0.0,1.0);
}

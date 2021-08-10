// view space axes
uniform vec3 _w;
uniform vec3 _u;
uniform vec3 _v;

uniform mat4 _cameraInverseProjection;
uniform vec3 _camh;
uniform vec3 _camv;
uniform vec3 _camll;
uniform vec3 _cpos;
uniform vec3 _tpos;

Ray createCameraRay(vec2 uv)
{
    // transform -1..1 -> 0..1
    uv = uv*0.5+0.5;

    vec3 target = vec3(0,0,0);

    vec3 dir;
    dir = uv.x*_camh + uv.y*_camv;
    dir = _camll + uv.x*_camh + uv.y*_camv;
    dir = normalize(dir);

    Ray ray = createRay(_cpos, dir);

    return ray;
};

uniform vec4 _seed;
layout(binding=1) uniform sampler2D _noise;                     // noise texture

vec4 sampleNoise(vec2 st)
{
    return texture(_noise, st);
}

float random(vec2 st)
{
    vec2 nuv = sampleNoise(st.xy).xy;

    return fract(sin(dot(nuv,vec2(12.9898,78.233)))*43758.5453123);
}

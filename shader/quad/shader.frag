#version 430 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

#define INV_SQRT_OF_2PI 0.39894228040143267793994605993439  // 1.0/SQRT_OF_2PI
#define INV_PI          0.31830988618379067153776752674503

// https://github.com/BrutPitt/glslSmartDeNoise/blob/master/Shaders/frag.glsl
vec4 denoise(sampler2D tex, vec2 uv, float sigma, float kSigma, float threshold)
{
    float radius = round(kSigma*sigma);
    float radQ = radius*radius;

    float invSigmaQx2 = .5 / (sigma*sigma);
    float invSigmaQx2PI = INV_PI * invSigmaQx2;

    float invThresholdSqx2 = .5 / (threshold*threshold);
    float invThresholdSqrt2PI = INV_SQRT_OF_2PI / threshold;

    vec4 centrPx = texture(tex,uv);

    float zBuff = 0.0;
    vec4 aBuff = vec4(0.0);
    vec2 size = vec2(textureSize(tex, 0));

    vec2 d;
    for (d.x=-radius; d.x <= radius; d.x++)
    {
        float pt = sqrt(radQ-d.x*d.x);
        for(d.y=-pt; d.y <= pt; d.y++)
        {
            float blurFactor = exp(-dot(d,d)*invSigmaQx2)*invSigmaQx2PI;

            vec4 walkPx = texture(tex,uv+d/size);

            vec4 dC = walkPx-centrPx;
            float deltaFactor = exp(-dot(dC.rgb,dC.rgb) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;

            zBuff += deltaFactor;
            aBuff += deltaFactor*walkPx;
        }
    }
    return aBuff/zBuff;
}

void main()
{
    float sigma = 2.2;
    float kSigma = 10.0;
    float threshold = 0.2;

    FragColor = denoise(ourTexture, TexCoord, sigma, kSigma, threshold);
    //FragColor = texture(ourTexture, TexCoord);
}

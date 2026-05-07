struct PixelShaderInput
{
    float4 clipPosition : SV_POSITION;
    float3 tint : TINT;
    float lifetime : LIFETIME;
    float2 uv : UV;
};

cbuffer PerParticleSystem : register(b0)
{
    float deltaTime;
    float lifetime;
    float atlasWidth;
    float atlasHeight;
};

//Texture2D diffuseTexture : register(t3);
//Texture2D alphaTexture : register(t7);

SamplerState samplerState : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    /* Alpha check */
    /*float alpha = alphaTexture.Sample(samplerState, input.uv).r;
    if (alpha < 0.5f)
    {
        discard;
    }*/

    /* Diffuse */
    //float3 color = diffuseTexture.Sample(samplerState, input.uv).rgb;
    float3 color = float3(1.0f, 1.0f, 1.0f) * input.tint;

	return float4(color, 1.0f);
}
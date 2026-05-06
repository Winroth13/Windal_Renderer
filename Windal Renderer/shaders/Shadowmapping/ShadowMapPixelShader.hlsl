// Input
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 worldNormal : WORLD_NORMAL;
    float2 uv : UV;
};

Texture2D alphaTexture : register(t7);

SamplerState samplerState : register(s0);

void main(PixelShaderInput input)
{
    float alpha = alphaTexture.Sample(samplerState, input.uv).r;
    
    if (alpha < 0.5f)
    {
        discard;
    }
    
    return;
}
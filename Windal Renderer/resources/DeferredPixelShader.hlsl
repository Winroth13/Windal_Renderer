struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 worldNormal : WORLD_NORMAL;
    float2 uv : UV;
};

struct PixelShaderOutput
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

// Constant buffers
cbuffer cbMateralIndex : register(b0)
{
    uint materialIndex;
    float3 pad0;
};

Texture2D diffuseTexture : register(t3);
SamplerState samplerState : register(s3);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.position = float4(input.worldPosition, 1.0f);
    output.normal = float4(input.worldNormal, 1.0f);
    output.color = float4(
        diffuseTexture.Sample(samplerState, input.uv).rgb,
        materialIndex
    );
    
    return output;
}
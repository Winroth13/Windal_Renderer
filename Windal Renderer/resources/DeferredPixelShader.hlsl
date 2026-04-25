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
cbuffer cbPerView : register(b1)
{
    float4x4 viewProj;
    float3 cameraPos;
    float pad3;
};

cbuffer cbPerMaterial : register(b3)
{
    float3 ambientCoefficient;
    float phongExponent;
    float3 diffuseCoefficient;
    float reflectiveness;
    float3 specularCoefficient;
    float pad7;
}

cbuffer cbMateralIndex : register(b4)
{
    uint materialIndex;
    float3 pad0;
};

Texture2D diffuseTexture : register(t3);
TextureCube cubemapTexture : register(t4);

SamplerState samplerState : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    output.position = float4(input.worldPosition, true);
    output.normal = float4(input.worldNormal, 1.0f);
    
    /* Diffuse */
    float3 color = diffuseTexture.Sample(samplerState, input.uv).rgb;
    
    /* Reflection */
    if (reflectiveness > 0)
    {
        float3 viewVector = input.worldPosition - cameraPos;
        float3 reflectVector = normalize(reflect(viewVector, input.worldNormal));
        float3 reflectionColor = cubemapTexture.Sample(samplerState, reflectVector).rgb;
    
        color = lerp(color, reflectionColor, reflectiveness);
    }
    
    output.color = float4(
        color, 
        materialIndex // Send material index to color gbuffer
    );
  
    return output;
}
// Input
struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

// Output
struct VertexShaderOutput
{
    float4 clipPosition : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 worldNormal : WORLD_NORMAL;
    float2 uv : UV;
};

// Constant buffers
cbuffer CameraBuffer : register(b0)
{
    matrix viewProjMatrix;
}

cbuffer EntityBuffer : register(b1)
{
    matrix worldMatrix;
}

VertexShaderOutput main(VertexShaderInput input)
{ 
    VertexShaderOutput output;
    
    float4 worldPos = mul(worldMatrix, float4(input.position, 1.0f));
    float4 clipPos = mul(viewProjMatrix, worldPos);

    output.clipPosition = clipPos;
    output.worldPosition = worldPos.xyz;

    output.worldNormal = mul(worldMatrix, float4(input.normal, 0.0f)).xyz;
    output.uv = input.uv;
    
    return output;
}
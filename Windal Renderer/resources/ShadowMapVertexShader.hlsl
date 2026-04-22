// Input
struct VertexShaderInput
{
    float3 position : POSITION;
};

// Output
struct VertexShaderOutput
{
    float4 clipPosition : SV_POSITION;
};

// Constant buffers
cbuffer cbPerView : register(b1)
{
    float4x4 viewProjMatrix;
    float3 cameraPos;
    float pad1;
}

cbuffer cbPerObject : register(b2)
{
    float4x4 worldMatrix;
    float4x4 worldInvTransposeMatrix;
};

VertexShaderOutput main(VertexShaderInput input) : SV_POSITION
{
    VertexShaderOutput output;
    
    float4 worldPos = mul(float4(input.position.xyz, 1.0f), worldMatrix);
    float4 clipPos = mul(viewProjMatrix, worldPos);

    output.clipPosition = clipPos;
    
    return output;
}
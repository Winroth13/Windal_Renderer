// Input
struct VertexShaderInput
{
    float3 position : POSITION;
    float3 color : COLOR;
};

// Output
struct VertexShaderOutput
{
    float4 clipPosition : SV_POSITION;
    float3 color : COLOR;
};

cbuffer cbPerView : register(b1)
{
    float4x4 viewProjMatrix;
    float3 cameraPos;
    float pad1;
}

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.clipPosition = mul(viewProjMatrix, float4(input.position, 1.0f));
    output.color = input.color;
    
    return output;
}
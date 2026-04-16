RWTexture2D<unorm float4> backBufferUAV;

Texture2D<float4> positionGBuffer : register(t0);
Texture2D<float4> normalGBuffer : register(t1);
Texture2D<float4> colorGBuffer : register(t2);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 position = positionGBuffer[DTid.xy].xyz;
    float3 normal = normalGBuffer[DTid.xy].xyz;
    float3 color = colorGBuffer[DTid.xy].xyz;
    
    backBufferUAV[DTid.xy] = float4(color.rgb, 1.0f);
}
// Input
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 worldNormal : WORLD_NORMAL;
    float2 uv : UV;
};

// Constant buffers
cbuffer cbPerFrame : register(b0)
{
    float3 sunDirection;
    float3 sunColor;
    float3 ambientColor;
    float3 pad0;
}

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

cbuffer cbPerMaterial : register(b3)
{
    float3 ambientCoefficient;
    float3 diffuseCoefficient;
    float3 specularCoefficient;
    float phongExponent;
    float2 padding;
}

Texture2D textures : register(t0);
SamplerState samplerState : register(s0);

bool useBlingPhong = true;

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Normalized surface normal
    float3 normal = normalize(input.worldNormal);
    // Normalized light direction
    float3 lightDir = normalize(-sunDirection);
    // Normalized view direction
    float3 viewDir = normalize(input.worldPosition - cameraPos);
    
    // Sample textures
    float4 ambDifTexture = textures.Sample(samplerState, input.uv);
    
    // Coefficients and exponents
    float3 cAmbient = ambientCoefficient * ambDifTexture.rgb;
    float3 cDiffuse = diffuseCoefficient * ambDifTexture.rgb;
    float3 cSpecular = specularCoefficient;
    
    // Light intensity
    int intensity = 1;
    float3 lightIntensity = sunColor * intensity;
    
    // Distance to light
    //float distance = length(input.worldPosition - lights[0].position);
    //float distance2 = distance * distance;
    float distance2 = 1;
    
    // Ambient light intensity
    float3 iAmbient = ambientColor;
    
    // Ambient light
    float3 ambientLight = cAmbient * iAmbient;
    
    // Diffuse light
    float NdotL = dot(normal, -lightDir);
    float diffuseIntensity = saturate(NdotL);
    float3 diffuseLight = cDiffuse * lightIntensity * diffuseIntensity / distance2;
    
    float specularIntensity;
    // Specular light
    if (useBlingPhong)
    {
        // Blinn-Phong reflectance model
        float3 halfVect = normalize(-lightDir + -viewDir);
        float NdotH = dot(normal, halfVect);
        specularIntensity = pow(saturate(NdotH), phongExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }
    else
    {
        // Blinn reflectance model
        float3 reflectVect = reflect(lightDir, normal);
        float RdotV = dot(reflectVect, -viewDir);
        specularIntensity = pow(saturate(RdotV), phongExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }
    
    float3 specularLight = cSpecular * lightIntensity * specularIntensity / distance2;
    
    float3 totalLight = ambientLight + diffuseLight + specularLight;
    
    //return float4(totalLight, 1.0f);
    
    return float4(ambDifTexture.rgb, 1.0f);
}
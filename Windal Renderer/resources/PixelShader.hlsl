// Input
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 worldNormal : WORLD_NORMAL;
    float2 uv : UV;
};

struct PointLight
{
    float3 position;
    float pad0;
    float3 color;
    float1 pad1;
    float range;
    float intensity;
    float2 pad3;
};

// Constant buffers
cbuffer cbPerFrame : register(b0)
{
    float3 sunDirection;
	float pad0;
    float3 sunColor;
    float pad1;
    float3 ambientColor;
    float pad2;
    int numPointLights;
    int numSpotLights;
    int numDirectionalLights;
    float pad3;
}

cbuffer cbPerView : register(b1)
{
    float4x4 viewProjMatrix;
    float3 cameraPos;
    float pad4;
}

cbuffer cbPerObject : register(b2)
{
    float4x4 worldMatrix;
    float4x4 worldInvTransposeMatrix;
};

cbuffer cbPerMaterial : register(b3)
{
    float3 ambientCoefficient;
    float pad5;
    float3 diffuseCoefficient;
    float pad6;
    float3 specularCoefficient;
    float phongExponent;
}

StructuredBuffer<PointLight> pointLights : register(t9);

Texture2D textures : register(t0);
SamplerState samplerState : register(s0);

bool useBlingPhong = true;

float3 getPhongPointLightColor(PointLight pointLight, float3 worldPosition, float3 normal, float2 uv)
{
    float3 lightIntensity = pointLight.color * pointLight.intensity;
    float distance = length(worldPosition - pointLight.position);
    float distanceSquare = distance * distance;
    float attenuation = pointLight.range * distanceSquare;
    
    float3 viewDir = normalize(worldPosition - cameraPos);
    float3 lightDir = normalize(worldPosition - pointLight.position);
    
    float4 texColor = textures.Sample(samplerState, uv);
    
    // coefficients and exponents
    float3 cDiffuse = diffuseCoefficient;
    float3 cSpecular = specularCoefficient;
    
    // diffuse light
    float NdotL = dot(normal, -lightDir);
    float diffuseIntensity = saturate(NdotL);
    float3 diffuseLight = cDiffuse * lightIntensity * diffuseIntensity / attenuation;
    
    float specularIntensity;
    // specular light
    if (useBlingPhong)
    {
        // Blinn-Phong reflectance model
        float3 halfVect = normalize(-lightDir + -viewDir);
        float NdotH = dot(normal, halfVect);
        specularIntensity = pow(saturate(NdotH), phongExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }
    else
    {
        // blinn reflectance model
        float3 reflectVect = reflect(lightDir, normal);
        float RdotV = dot(reflectVect, -viewDir);
        specularIntensity = pow(saturate(RdotV), phongExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }
    
    float3 specularLight = cSpecular * lightIntensity * specularIntensity / attenuation;
    float3 totalLight = diffuseLight + specularLight;
    return totalLight;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 totalLight;
    
    /* ambient */
    float4 ambDifTexture = textures.Sample(samplerState, input.uv);
    float3 iAmbient = ambientColor;
    float3 cAmbient = ambientCoefficient * ambDifTexture.rgb;
    float3 ambientLight = cAmbient * iAmbient;
    
    totalLight = ambientLight;
    
    /* point lights */
    for (int i = 0; i < numPointLights; ++i)
    {
        totalLight += getPhongPointLightColor(
            pointLights[i], 
            input.worldPosition, 
            normalize(input.worldNormal),
            input.uv
        );
    }
    
    return float4(totalLight.rgb, 1.0f);
}
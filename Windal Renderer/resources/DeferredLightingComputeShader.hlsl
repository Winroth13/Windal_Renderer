RWTexture2D<unorm float4> backBufferUAV;

Texture2D<float4> positionGBuffer : register(t4);
Texture2D<float4> normalGBuffer : register(t5);
Texture2D<float4> colorGBuffer : register(t6);

// Lights
struct DirectionalLight
{
    float3 direction;
    float intensity;
    float3 color;
    float pad0;
};

struct PointLight
{
    float3 position;
    float attenuation;
    float3 color;
    float intensity;
};

struct SpotLight
{
    float3 position;
    float attenuation;
    float3 color;
    float intensity;
    float3 direction;
    float angle;
};

// Material
struct Material
{
    float3 ambientCoefficient;
    float pad0;
    float3 diffuseCoefficient;
    float pad1;
    float3 specularCoefficient;
    float phongExponent;
};

// Constant buffers
cbuffer cbPerFrame : register(b0)
{
    float3 ambientColor;
    int numDirectionalLights;
    int numPointLights;
    int numSpotLights;
    int usingBlinnPhong;
    float pad0;
};

cbuffer cbPerView : register(b1)
{
    float4x4 pad1;
    float3 cameraPos;
    float pad2;
};

StructuredBuffer<DirectionalLight> directionalLights : register(t0);
StructuredBuffer<PointLight> pointLights : register(t1);
StructuredBuffer<SpotLight> spotLights : register(t2);
StructuredBuffer<Material> materials : register(t3);

float3 CalculateLightColor(
    float3 color,
    float intensity,
    float attenuation,
    float distance,
    float3 lightDir,
    float3 worldPosition,
    float3 worldNormal,
    float3 diffuseColor,
    uint materialIndex)
{
    float3 lightIntensity = color * intensity;
    attenuation = attenuation * pow(distance, 2);

    float3 viewDir = normalize(worldPosition - cameraPos);
    lightDir = normalize(lightDir);

    worldNormal = normalize(worldNormal);

    // coefficients and exponents
    float3 cDiffuse = materials[materialIndex].diffuseCoefficient * diffuseColor.rgb;
    float3 cSpecular = materials[materialIndex].specularCoefficient;

    // diffuse light
    float NdotL = dot(worldNormal, -lightDir);
    float diffuseIntensity = saturate(NdotL);
    float3 diffuseLight = cDiffuse * lightIntensity * diffuseIntensity / attenuation;

    float specularIntensity;
    // specular light
    if (usingBlinnPhong)
    {
        // Blinn-Phong reflectance model
        float3 halfVect = normalize(-lightDir + -viewDir);
        float NdotH = dot(worldNormal, halfVect);
        specularIntensity = pow(saturate(NdotH), materials[materialIndex].phongExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }
    else
    {
        // Blinn reflectance model
        float3 reflectVect = reflect(lightDir, worldNormal);
        float RdotV = dot(reflectVect, -viewDir);
        specularIntensity = pow(saturate(RdotV), materials[materialIndex].phongExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }

    float3 specularLight = cSpecular * lightIntensity * specularIntensity / attenuation;
    float3 totalLight = diffuseLight + specularLight;
    return totalLight;
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{   
    float3 worldPosition = positionGBuffer[DTid.xy].xyz;
    float3 worldNormal = normalGBuffer[DTid.xy].xyz;
    float3 texColor = colorGBuffer[DTid.xy].rgb;
    uint materialIndex = colorGBuffer[DTid.xy].a;
    
    float3 totalLight;
    
    /* ambient */
    float3 iAmbient = ambientColor;
    float3 cAmbient = materials[materialIndex].ambientCoefficient * texColor.rgb;
    float3 ambientLight = cAmbient * iAmbient;
    
    totalLight = ambientLight;
    
    /* Directional lights */
    for (int i = 0; i < numDirectionalLights; ++i)
    {
        totalLight += CalculateLightColor(
            directionalLights[i].color,
            directionalLights[i].intensity,
            1,
            1,
            directionalLights[i].direction,
            worldPosition,
            worldNormal,
            texColor.rgb,
            materialIndex
        );
    }
    
    /* Point lights */
    for (i = 0; i < numPointLights; ++i)
    {
        totalLight += CalculateLightColor(
            pointLights[i].color,
            pointLights[i].intensity,
            pointLights[i].attenuation,
            length(worldPosition - pointLights[i].position),
            worldPosition - pointLights[i].position,
            worldPosition,
            worldNormal,
            texColor.rgb,
            materialIndex
        );
    }
    
    /* Spot lights */
    for (i = 0; i < numSpotLights; ++i)
    {
        float3 lightDir = (worldPosition - spotLights[i].position);
        float spotFactor = dot(normalize(lightDir), normalize(spotLights[i].direction));
        float cutoff = cos(spotLights[i].angle);

        if (spotFactor > cutoff)
        {
            float3 color = CalculateLightColor(
                spotLights[i].color,
                spotLights[i].intensity,
                spotLights[i].attenuation,
                length(lightDir),
                lightDir,
                worldPosition,
                worldNormal,
                texColor.rgb,
                materialIndex
            );

            // Smoothes out the outer 20% of the light
            // TODO: Add an outer angle to decide what the "outer percent" is
            float falloffFactor = (1.0 - (1.0 - spotFactor) / (1.0 - cutoff)) * 5;
            falloffFactor = min(falloffFactor, 1);

            color = color * falloffFactor;

            totalLight += color;
        }
    }
    
    backBufferUAV[DTid.xy] = float4(totalLight.rgb, 1.0f);
}
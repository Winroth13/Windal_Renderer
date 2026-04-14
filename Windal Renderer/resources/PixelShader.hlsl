// Input
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 worldNormal : WORLD_NORMAL;
    float2 uv : UV;
};

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

// Constant buffers
cbuffer cbPerFrame : register(b0)
{
    float3 ambientColor;
    int numDirectionalLights;
    int numPointLights;
    int numSpotLights;
    int usingBlinnPhong;
    float pad4;
}

cbuffer cbPerView : register(b1)
{
    float4x4 viewProjMatrix;
    float3 cameraPos;
    float pad5;
}

cbuffer cbPerObject : register(b2)
{
    float4x4 worldMatrix;
    float4x4 worldInvTransposeMatrix;
};

cbuffer cbPerMaterial : register(b3)
{
    float3 ambientCoefficient;
    float pad6;
    float3 diffuseCoefficient;
    float pad7;
    float3 specularCoefficient;
    float phongExponent;
}

StructuredBuffer<DirectionalLight> directionalLights : register(t0);
StructuredBuffer<PointLight> pointLights : register(t1);
StructuredBuffer<SpotLight> spotLights : register(t2);

Texture2D textures : register(t3);
SamplerState samplerState : register(s3);

float3 CalculateLightColor(
    float3 color,
    float intensity,
    float attenuation,
    float distance,
    float3 lightDir,
    float3 worldPosition,
    float3 worldNormal, 
    float3 diffuseColor)
{
    float3 lightIntensity = color * intensity;
    attenuation = attenuation * pow(distance, 2);

    float3 viewDir = normalize(worldPosition - cameraPos);
    lightDir = normalize(lightDir);

    worldNormal = normalize(worldNormal);

    // coefficients and exponents
    float3 cDiffuse = diffuseCoefficient * diffuseColor.rgb;
    float3 cSpecular = specularCoefficient;

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
        specularIntensity = pow(saturate(NdotH), phongExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }
    else
    {
        // Blinn reflectance model
        float3 reflectVect = reflect(lightDir, worldNormal);
        float RdotV = dot(reflectVect, -viewDir);
        specularIntensity = pow(saturate(RdotV), phongExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }

    float3 specularLight = cSpecular * lightIntensity * specularIntensity / attenuation;
    float3 totalLight = diffuseLight + specularLight;
    return totalLight;
}

float easeInExpo(float number)
{
    return pow(2, 10 * number - 10);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 totalLight;
    
    /* ambient */
    float4 texColor = textures.Sample(samplerState, input.uv);
    float3 iAmbient = ambientColor;
    float3 cAmbient = ambientCoefficient * texColor.rgb;
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
            input.worldPosition,
            input.worldNormal,
            texColor.rgb
        );
    }
    
    /* Point lights */
    for (i = 0; i < numPointLights; ++i)
    {
        totalLight += CalculateLightColor(
            pointLights[i].color,
            pointLights[i].intensity,
            pointLights[i].attenuation,
            length(input.worldPosition - pointLights[i].position),
            input.worldPosition - pointLights[i].position,
            input.worldPosition,
            input.worldNormal,
            texColor.rgb
        );
    }
    
    /* Spot lights */
    for (i = 0; i < numSpotLights; ++i)
    {  
        float3 lightDir = (input.worldPosition - spotLights[i].position);
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
                input.worldPosition,
                input.worldNormal,
                texColor.rgb
            );

            // Smoothes out the outer 20% of the light
            // TODO: Add an outer angle to decide what the "outer percent" is
            float falloffFactor = (1.0 - (1.0 - spotFactor) / (1.0 - cutoff)) * 5;
            falloffFactor = min(falloffFactor, 1);

            color = color * falloffFactor;

            totalLight += color;
        }
    }
   
    return float4(totalLight.rgb, 1.0f);
}
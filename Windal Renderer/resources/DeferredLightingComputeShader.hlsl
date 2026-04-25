RWTexture2D<unorm float4> backBufferUAV;

Texture2D<float4> positionGBuffer : register(t7);
Texture2D<float4> normalGBuffer : register(t8);
Texture2D<float4> colorGBuffer : register(t9);

// Lights
struct DirectionalLight
{
    float4x4 viewProjMatrix;
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
    float4x4 viewProjMatrix;
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
    int flags;
    float pad0;
    uint2 screenDimensions;
    float2 pad1;
};

cbuffer cbPerView : register(b1)
{
    float4x4 pad2;
    float3 cameraPos;
    float pad3;
};

StructuredBuffer<DirectionalLight> directionalLights : register(t0);
StructuredBuffer<PointLight> pointLights : register(t1);
StructuredBuffer<SpotLight> spotLights : register(t2);
StructuredBuffer<Material> materials : register(t3);

Texture2DArray<float> directionalLightShadowMaps : register(t4);
Texture2DArray<float> pointLightShadowMaps : register(t5);
Texture2DArray<float> spotLightShadowMaps : register(t6);

sampler shadowMapSampler : register(s1);

#define WIRE_FRAME 1
#define	SHOW_GBUFFERS 2
#define	USE_BLINN_PHONG 4

#define SHADOW_MAP_BIAS 0.001
#define SHADOW_SAMPLES_DIMENTIONS 3

float calcShadowFactor(
    float3 fragmentWorldPosition,
    float4x4 lightViewProjMatrix,
    int index,
    const Texture2DArray<float> texArr)
{
    float4 lightClipPos = mul(lightViewProjMatrix, float4(fragmentWorldPosition, 1.0f));
    float3 NDC = lightClipPos.xyz / lightClipPos.w;
    float fragmentDepth = NDC.z;
    
    float2 uv = float2((NDC.x * 0.5) + 0.5, (-NDC.y * 0.5) + 0.5);
    
    float factor = 0.0f;
    
    uint width = -1;
    uint height = -1;
    uint elements = -1;
    uint nLevels = -1;
    texArr.GetDimensions(index, width, height, elements, nLevels);
    
    int numSamples = 0;
    
    for (int y = -SHADOW_SAMPLES_DIMENTIONS; y <= SHADOW_SAMPLES_DIMENTIONS; y++)
    {
        for (int x = -SHADOW_SAMPLES_DIMENTIONS; x <= SHADOW_SAMPLES_DIMENTIONS; x++)
        {
            float2 offsets = float2(x * (1.0f / width), y * (1.0f / height));
            float3 uvc = float3(uv + offsets, index);
            float depth = texArr.SampleLevel(shadowMapSampler, uvc, 0);
            if (fragmentDepth > (depth + SHADOW_MAP_BIAS))
            {
                factor += 0.0f;
            }
            else
            {
                factor += 1.0f;
            }
            numSamples++;
        }
    }
    
    return (factor / numSamples);
}

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
    if ((flags & USE_BLINN_PHONG) == USE_BLINN_PHONG)
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
void main( uint3 DTid : SV_DispatchThreadID)
{   
    uint2 screenPos = DTid.xy;
    float2 screenUV = float2(DTid.x / (float) screenDimensions.x, DTid.y / (float) screenDimensions.y);

    float3 worldPosition = positionGBuffer[screenPos].xyz;
    float3 worldNormal = normalGBuffer[screenPos].xyz;
    float3 texColor = colorGBuffer[screenPos].rgb;
    uint materialIndex = colorGBuffer[screenPos].a;
    bool isSomething = positionGBuffer[screenPos].w;
    
    if ((flags & SHOW_GBUFFERS) == SHOW_GBUFFERS)
    {
        uint topY = screenPos.y;
        uint bottomY = screenPos.y - screenDimensions.y / 2;
        uint leftX = screenPos.x;
        uint rightX = screenPos.x - screenDimensions.x / 2;
        // Top-half
        if (screenUV.y < 0.5)
        {
            // Top left
            if (screenUV.x < 0.5)
            {
                backBufferUAV[screenPos] = float4(positionGBuffer[uint2(leftX, topY) * 2].rgb, 1.0f);
                return;
            }
            // Top right
            isSomething = positionGBuffer[uint2(rightX, topY) * 2].w;
            if (!isSomething)
            {
                backBufferUAV[screenPos] = float4(0.0f, 0.0f, 0.0f, 1.0f);
                return;
            }
            backBufferUAV[screenPos] = float4(normalGBuffer[uint2(rightX, topY) * 2].xyz, 1.0f);
            return;
        }
        else if (screenUV.x < 0.5)
        {
            // Bottom left
            backBufferUAV[screenPos] = float4(colorGBuffer[uint2(leftX, bottomY) * 2].rgb, 1.0f);
            return;
        }
        // Bottom right
        uint2 offsetScreenPos = uint2(rightX, bottomY) * 2;
        worldPosition = positionGBuffer[offsetScreenPos].xyz;
        worldNormal = normalGBuffer[offsetScreenPos].xyz;
        texColor = colorGBuffer[offsetScreenPos].rgb;
        materialIndex = colorGBuffer[offsetScreenPos].a;
        isSomething = positionGBuffer[offsetScreenPos].w;
    }

    if (!isSomething)
    {
        return;
    }
    
    float3 totalLight;
    
    /* ambient */
    float3 iAmbient = ambientColor;
    float3 cAmbient = materials[materialIndex].ambientCoefficient * texColor.rgb;
    float3 ambientLight = cAmbient * iAmbient;
    
    totalLight = ambientLight;
    
    /* Directional lights */
    for (int i = 0; i < numDirectionalLights; ++i)
    {
        float shadowFactor = calcShadowFactor(
            worldPosition,
            directionalLights[i].viewProjMatrix,
            i,
            directionalLightShadowMaps
        );
        
        if (shadowFactor != 0)
        {
            float3 color = CalculateLightColor(
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
        
            totalLight += color * shadowFactor;
        }
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
            float shadowFactor = calcShadowFactor(
                worldPosition,
                spotLights[i].viewProjMatrix,
                i,
                spotLightShadowMaps
            );

            if (shadowFactor != 0)
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

                color = color * falloffFactor * shadowFactor;

                totalLight += color;
            }
        }
    }

    backBufferUAV[screenPos] = float4(totalLight.rgb, 1.0f);
}
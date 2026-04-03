// Input
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float3 worldNormal : WORLD_NORMAL;
    float2 uv : UV;
};

// A directional light
struct DirectionalLight
{
    float3 direction;
    float pad0;
    float3 colour;
    float intensity;
};

// A point light
struct PointLight
{
    float3 position;
    float pad0;
    float3 colour;
    float intensity;
};

// Object light properties
struct ObjectProperties
{
    float3 ambient;
    float pad0;
	float3 diffuse;
    float pad1;
    float3 specular;
    int specularExp;
};

// Constant buffers
cbuffer CameraBuffer : register(b0)
{
    float3 cameraPosition;
}

cbuffer ObejctProperties : register(b1)
{
    ObjectProperties properties;
}

cbuffer LightBuffer : register(b2)
{
    PointLight lights[1];
}

Texture2D textures : register(t0);
SamplerState samplerState : register(s0);

bool useBlingPhong = true;

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Normalized surface normal
    float3 normal = normalize(input.worldNormal);
    // Normalized light direction
    float3 lightDir = normalize(input.worldPosition - lights[0].position);
    // Normalized view direction
    float3 viewDir = normalize(input.worldPosition - cameraPosition);
    
    // Sample textures
    float4 ambDifTexture = textures.Sample(samplerState, input.uv);
    
    // Coefficients and exponents
    float3 cAmbient = properties.ambient * ambDifTexture.rgb;
    float3 cDiffuse = properties.diffuse * ambDifTexture.rgb;
    float3 cSpecular = properties.specular;
    float specularExponent = properties.specularExp;
    
    // Light intensity
    float3 lightIntensity = lights[0].colour * lights[0].intensity;
    
    // Distance to light
    float distance = length(input.worldPosition - lights[0].position);
    float distance2 = distance * distance;
    
    // Ambient light intensity
    float3 iAmbient = { 0.1f, 0.1f, 0.1f };
    
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
        specularIntensity = pow(saturate(NdotH), specularExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }
    else
    {
        // Blinn reflectance model
        float3 reflectVect = reflect(lightDir, normal);
        float RdotV = dot(reflectVect, -viewDir);
        specularIntensity = pow(saturate(RdotV), specularExponent); // pow(0, 0) is undefined behaviour, but is prevented in Mesh.cpp
    }
    
    float3 specularLight = cSpecular * lightIntensity * specularIntensity / distance2;
    
    float3 totalLight = ambientLight + diffuseLight + specularLight;
    
    return float4(totalLight, 1.0f);
}
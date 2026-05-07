cbuffer PerParticleSystem : register(b0)
{
    float deltaTime;
    float lifetime;
    float atlasWidth;
    float atlasHeight;
};

struct Particle
{
    float3 position;
    float scale;
    float3 tint;
    float lifetime;
};

RWStructuredBuffer<Particle> Particles : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Particle particle = Particles[DTid.x];
    
    particle.position.y += 0.001f;
    
    Particles[DTid.x] = particle;
}
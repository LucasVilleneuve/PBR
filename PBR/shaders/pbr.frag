#version 450 core

in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 LightPosition; 
uniform vec3 LightColor; 

uniform vec3 CamPos;

uniform float Roughness;
uniform float Metallic;
uniform bool GammaCorr;
//uniform vec3 Kd;
//uniform vec3 Ka;
//uniform vec3 Ks;
//uniform float shininess;

out vec4 finalColor;

const float PI = 3.1415926535897932384626433832795;

vec3 FresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;

    float denom = NdotH2 * (a2 - 1.0) + 1.0;
	
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float k)
{
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Direct lightning

    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 HDRToneMapping(vec3 color)
{
	return color / (color + vec3(1.0));
}

vec3 GammaCorrection(vec3 color)
{
	float gamma = 2.2;
	return pow(color, vec3(1.0 / gamma)); 
}

void main()
{   
	vec3 N = normalize(Normal);
	vec3 V = normalize(CamPos - Position);
	vec3 L = normalize(LightPosition - Position); // TODO Need to calculate L for each lights
	vec3 H = normalize(V + L);
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float NdotH = max(dot(N, H), 0.0);
	float HdotV = max(dot(H, V), 0.0);

	vec3 albedo = vec3(1.0, 0.0, 0.0);
	float ao = 1.0;

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, Metallic);

	vec3 lo = vec3(0.0);

	// calculate per-light radiance
    float dist = length(LightPosition - Position);
    float attenuation = 1.0 / (dist * dist);
    vec3 radiance = LightColor * attenuation;
	
	// cook-torrance brdf
	float d = DistributionGGX(NdotH, Roughness);
	float g = GeometrySmith(NdotV, NdotL, Roughness);
	vec3 f = FresnelSchlick(HdotV, f0);
        
    vec3 kS = f;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - Metallic;

    vec3 specular = (d * g * f) / max(4.0 * NdotV * NdotL, 0.001);

    // add to outgoing radiance Lo
    lo += (kD * albedo / PI + specular) * radiance * NdotL; 

	vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + lo;

	if (GammaCorr)
	{
		color = HDRToneMapping(color);
		color = GammaCorrection(color);
	}

	finalColor = vec4(color, 1.0);
}
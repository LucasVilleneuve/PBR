#version 450 core

in vec3 Position;
in vec3 Normal;
in vec2 TexCoords;

// lights
const int NUMBER_OF_LIGHTS = 4;
uniform vec3 LightPositions[NUMBER_OF_LIGHTS];
uniform vec3 LightColors[NUMBER_OF_LIGHTS];

uniform vec3 CamPos;

uniform float Roughness;
uniform float Metallic;
uniform bool GammaCorr;
uniform sampler2D AlbedoMap;
uniform sampler2D NormalMap;
uniform sampler2D MetallicMap;
uniform sampler2D RoughnessMap;
uniform sampler2D AoMap;

out vec4 finalColor;

const float PI = 3.1415926535897932384626433832795;
const float GAMMA = 2.2;

vec3 GetNormalFromNormalMap()
{
    vec3 tangentNormal = texture(NormalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(Position);
    vec3 Q2  = dFdy(Position);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

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
	return pow(color, vec3(1.0 / GAMMA)); 
}

void main()
{
	vec3 albedo = pow(texture(AlbedoMap, TexCoords).rgb, vec3(GAMMA));
    vec3 normal = GetNormalFromNormalMap();
	float metallic = texture(MetallicMap, TexCoords).r;
    float roughness = texture(RoughnessMap, TexCoords).r;
//    float ao        = texture(aoMap, TexCoords).r;

//	float metallic = Metallic;
//	float roughness = Roughness;

	vec3 N = normalize(normal);
	vec3 V = normalize(CamPos - Position);
	float NdotV = max(dot(N, V), 0.0);

//	vec3 albedo = vec3(0.8, 0.0, 0.0);
	float ao = 1.0;

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);

	vec3 lo = vec3(0.0);

	for (int i = 0; i < NUMBER_OF_LIGHTS; ++i)
	{
		vec3 L = normalize(LightPositions[i] - Position);
		vec3 H = normalize(V + L);
		float NdotL = max(dot(N, L), 0.0);
		float NdotH = max(dot(N, H), 0.0);
		float HdotV = max(dot(H, V), 0.0);

		// calculate per-light radiance
		float dist = length(LightPositions[i] - Position);
		float attenuation = 1.0 / (dist * dist + 0.001);
		vec3 radiance = LightColors[i] * attenuation;
	
		// cook-torrance brdf
		float d = DistributionGGX(NdotH, roughness);
		float g = GeometrySmith(NdotV, NdotL, roughness);
		vec3 f = FresnelSchlick(HdotV, f0);
        
		vec3 specular = (d * g * f) / (4.0 * NdotV * NdotL + 0.001);

		vec3 kS = f;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		// add to outgoing radiance Lo
		lo += (kD * albedo / PI + specular) * radiance * NdotL; 
	}

	vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + lo;
	
	if (GammaCorr)
	{
		color = HDRToneMapping(color);
		color = GammaCorrection(color);
	}

	finalColor = vec4(color, 1.0);
}
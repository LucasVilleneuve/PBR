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
uniform samplerCube irradianceMap;
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
	
    return a2 / max((PI * denom * denom), 0.001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
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
	vec3 albedo     = pow(texture(AlbedoMap, TexCoords).rgb, vec3(2.2));
    float metallic  = texture(MetallicMap, TexCoords).r;
    float roughness = texture(RoughnessMap, TexCoords).r;
    float ao        = texture(AoMap, TexCoords).r;

    vec3 N = GetNormalFromNormalMap();
    vec3 V = normalize(CamPos - Position);
	vec3 R = reflect(-V, N);
	float NdotV = max(dot(N, V), 0.0);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(LightPositions[i] - Position);
        vec3 H = normalize(V + L);
		float NdotL = max(dot(N, L), 0.0);
		float NdotH = max(dot(N, H), 0.0);
		float HdotV = max(dot(H, V), 0.0);

        float distance = length(LightPositions[i] - Position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = LightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(NdotH, roughness);   
        float G   = GeometrySmith(NdotV, NdotL, roughness);
        vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 kS = FresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;
    // vec3 ambient = vec3(0.002);
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

//	color = diffuse;
    finalColor = vec4(color, 1.0);
}
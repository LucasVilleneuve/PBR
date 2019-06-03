#version 450 core

//in vec2 TexCoords;
in vec3 Normal;
in vec3 Position;

uniform vec3 LightPosition; 
uniform vec3 LightColor; 
uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float shininess;

out vec4 color;

vec3 gooch()
{
	vec3  SurfaceColor = vec3(0.4); // (0.75, 0.75, 0.75)
	vec3  WarmColor = vec3(1.0, 1.0, 1.0);    // (0.6, 0.6, 0.0)
	vec3  CoolColor = vec3(0.1, 0.1, 0.1);    // (0.0, 0.0, 0.6)
	float DiffuseWarm = 0.45;  // 0.45
	float DiffuseCool = 0.45; // 0.45

 	vec3 L = normalize(LightPosition - Position);
	vec3 V = normalize(vec3(-Position));
	vec3 N = normalize(Normal);
	vec3 H = normalize(V + L);

	float NdotL = max(dot(N, L), 0.0f);

    vec3 kcool    = min(CoolColor + DiffuseCool * SurfaceColor, 1.0);
    vec3 kwarm    = min(WarmColor + DiffuseWarm * SurfaceColor, 1.0); 
    vec3 kfinal   = mix(kcool, kwarm, NdotL);

	vec3 ReflectVec = normalize(reflect(-L, Normal));
	vec3 nreflect = normalize(ReflectVec);
    vec3 nview    = normalize(V);

    float spec    = max(dot(nreflect, nview), 0.0);
	spec = pow(spec, 32.0);

	return min(kfinal + spec, 1.0);
}

vec3 phong(vec3 position, vec3 norm)
{
	vec3 L = normalize(LightPosition - position);
	vec3 V = normalize(vec3(-position));
	vec3 N = normalize(norm);
	vec3 H = normalize(V + L);

	vec3 ambiant = Ka; // Do we need to multiply by LightColor?
	vec3 diffuse = Kd * LightColor * max(dot(L, N), 0.0f);
	vec3 specular = Ks * LightColor * pow(max(dot(H, N), 0.0f), shininess);

	return ambiant + diffuse + specular;
}

void main()
{   
	vec3 Color = phong(Position, Normal);
    color = vec4(gooch(), 1.0);
}
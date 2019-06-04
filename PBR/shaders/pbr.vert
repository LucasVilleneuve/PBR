#version 450

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 TexCoordinates;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat4 Model;

uniform mat3 NormalMatrix;

void main()
{
    Position = (ModelView * vec4(VertexPosition, 1.0)).xyz;
    Normal = normalize(NormalMatrix * VertexNormal);
	TexCoords = TexCoordinates;

    gl_Position =  Projection * vec4(Position, 1.0f);
}
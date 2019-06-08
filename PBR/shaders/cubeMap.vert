#version 450

layout (location = 0) in vec3 VertexPosition;

out vec3 WorldPos;

uniform mat4 Projection;
uniform mat4 ModelView;

void main()
{
    WorldPos = VertexPosition;  
    gl_Position =  Projection * ModelView * vec4(WorldPos, 1.0);
}
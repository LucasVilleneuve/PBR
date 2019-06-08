#version 450

layout (location = 0) in vec3 VertexPosition;

uniform mat4 Projection;
uniform mat4 ModelView;

out vec3 WorldPos;

void main()
{
    WorldPos = VertexPosition;

	mat4 rotView = mat4(mat3(ModelView));
	vec4 clipPos = Projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}
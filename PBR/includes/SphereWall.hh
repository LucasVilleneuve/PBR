#pragma once

#include "Shader.hh"
#include "Sphere.hh"

class SphereWall
{
public:
	SphereWall(Shader &shader);
	~SphereWall() = default;

	void draw(const glm::mat4 &view);

private:
	const int NB_COLUMNS = 3;
	const int NB_ROWS = 3;

	Shader &_shader;
	std::vector<Sphere> _wall;
	float _xSpacing = 2.5f;
	float _ySpacing = 2.5f;

	// TODO Change that
	GLuint albedo;
	GLuint metallic;
	GLuint roughness;
	GLuint normal;
};
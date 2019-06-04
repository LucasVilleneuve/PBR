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
	const int NB_COLUMNS = 11;
	const int NB_ROWS = 11;

	Shader &_shader;
	std::vector<Sphere> _wall;
	float _xOffset = 2.5f;
	float _yOffset = 2.5f;
};
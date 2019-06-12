#pragma once

#include <functional>
#include "Shader.hh"
#include "TexturedSphere.hh"
#include "SimpleSphere.hh"
#include "TextureLoader.hh"

class SphereWall
{
public:
	SphereWall();
	~SphereWall() = default;

	void draw(Shader &shader, const glm::mat4 &view, const std::function<void()> &addTextures);

private:
	const int NB_COLUMNS = 3;
	const int NB_ROWS = 3;

	//std::vector<TexturedSphere> _wall;
	std::vector<SimpleSphere> _wall;
	float _xSpacing = 2.5f;
	float _ySpacing = 2.5f;
};
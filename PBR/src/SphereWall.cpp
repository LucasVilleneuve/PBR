#include "SphereWall.hh"

SphereWall::SphereWall(Shader &shader)
	: _shader(shader)
{
	float metallic = 0.0;

	float xStep = (1.0 / NB_COLUMNS);
	float yStep = (1.0 / NB_ROWS);

	for (int y = 0; y < NB_ROWS; ++y)
	{
		float yPos = _ySpacing * (y % NB_ROWS);
		float roughness = 0.0;

		for (int x = 0; x < NB_COLUMNS; ++x)
		{
			float xPos = _xSpacing * (x % NB_COLUMNS);

			TexturedSphere sphere("resources/plastic/", _shader, 1);
			//SimpleSphere sphere(_shader, roughness, metallic, 1);

			// Setting Model
			Model model = sphere.getModel();
			model.glTranslate(xPos, yPos, 0);
			model.glRotate(90, 1, 0, 0);
			sphere.setModel(model);

			_wall.push_back(sphere);

			roughness += xStep;
		}

		metallic += yStep;
	}
}

void SphereWall::draw(const glm::mat4 &view, const std::function<void()> &addTextures)
{
	addTextures();

	for (auto sphere : _wall)
	{
		sphere.draw(view);
	}

	glBindTexture(GL_TEXTURE_2D, 0);  //de-activate
}
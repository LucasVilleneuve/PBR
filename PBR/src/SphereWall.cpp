#include "SphereWall.hh"

SphereWall::SphereWall(Shader &shader)
	: _shader(shader)
{
	float metallic = 0.0;

	for (int y = 0; y < NB_ROWS; ++y)
	{
		float yPos = _yOffset * (y % NB_ROWS);
		float roughness = 0.0;

		for (int x = 0; x < NB_COLUMNS; ++x)
		{
			float xPos = _xOffset * (x % NB_COLUMNS);

			Sphere sphere("", 1);

			// Setting Model
			Model model = sphere.getModel();
			model.glTranslate(xPos, yPos, 0);
			sphere.setModel(model);

			// Setting attributes
			sphere.setRoughness(roughness);
			sphere.setMetallic(metallic);

			_wall.push_back(sphere);

			roughness += 0.1;
		}

		metallic += 0.1;
	}
}

void SphereWall::draw(const glm::mat4 &view)
{
	glm::mat4 mview;
	glm::mat4 imvp;
	glm::mat3 nmat;

	for (auto sphere : _wall)
	{
		mview = view * sphere.getModel().getMatrix();
		imvp = glm::inverse(mview);
		nmat = glm::mat3(glm::transpose(imvp));

		//_shader->setVec3("Kd", sphere.getKd());
		_shader.setMat4("ModelView", mview);
		_shader.setMat3("NormalMatrix", nmat);

		_shader.setFloat("Roughness", sphere.getRoughness());
		_shader.setFloat("Metallic", sphere.getMetallic());

		sphere.draw();
	}
}
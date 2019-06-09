#include "SphereWall.hh"

SphereWall::SphereWall(Shader &shader)
	: _shader(shader)
{
	albedo = Sphere::loadTexture("resources/rusted_iron/albedo.png");
	metallic = Sphere::loadTexture("resources/rusted_iron/metallic.png");
	roughness = Sphere::loadTexture("resources/rusted_iron/roughness.png");
	normal = Sphere::loadTexture("resources/rusted_iron/normal.png");

	for (int y = 0; y < NB_ROWS; ++y)
	{
		float yPos = _ySpacing * (y % NB_ROWS);

		for (int x = 0; x < NB_COLUMNS; ++x)
		{
			float xPos = _xSpacing * (x % NB_COLUMNS);

			Sphere sphere("", 1);

			// Setting Model
			Model model = sphere.getModel();
			model.glTranslate(xPos, yPos, 0);
			model.glRotate(90, 1, 0, 0);
			sphere.setModel(model);

			// Setting attributes
			sphere.setAlbedo(albedo);
			sphere.setRoughness(roughness);
			sphere.setMetallic(metallic);

			_wall.push_back(sphere);
		}
	}
}

void SphereWall::draw(const glm::mat4 &view, const std::function<void()> &addTextures)
{
	glm::mat4 mview;
	glm::mat4 imvp;
	glm::mat3 nmat;

	_shader.setInt("AlbedoMap", 0);
	_shader.setInt("NormalMap", 1);
	_shader.setInt("MetallicMap", 2);
	_shader.setInt("RoughnessMap", 3);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, metallic);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, roughness);
	addTextures();

	for (auto sphere : _wall)
	{
		mview = view * sphere.getModel().getMatrix();
		imvp = glm::inverse(mview);
		nmat = glm::mat3(glm::transpose(imvp));

		//_shader->setVec3("Kd", sphere.getKd());
		_shader.setMat4("ModelView", mview);
		_shader.setMat3("NormalMatrix", nmat);

		//_shader.setFloat("Roughness", 0.2);
		//_shader.setFloat("Metallic", 0.5);
		//_shader.setInt("RoughnessMap", sphere.getRoughness());
		//_shader.setInt("MetallicMap", sphere.getMetallic());

		sphere.draw();
	}

	glBindTexture(GL_TEXTURE_2D, 0);  //de-activate
}
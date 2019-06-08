#include "Light.hh"

Light::Light(int number, Shader &shader, glm::vec3 position, glm::vec3 color)
	: _number(number), _shader(shader), _sphere("", 1)
{
	this->setPosition(position);
	this->setColor(color);
}

void Light::setPosition(glm::vec3 pos)
{
	if (_pos == pos) return;

	Model m;
	m.glTranslate(pos.x, pos.y, pos.z);
	_sphere.setModel(m);
	_pos = pos;
}

void Light::draw(const glm::mat4 &view)
{
	// Set light properties
	glm::vec3 pos = glm::vec3(view * glm::vec4(_pos, 1.0));
	_shader.setVec3("LightPositions[" + std::to_string(_number) + "]", pos);
	_shader.setVec3("LightColors[" + std::to_string(_number) + "]", _color);

	// Draw sphere
	glm::mat4 mview = view * _sphere.getModel().getMatrix();
	glm::mat4 imvp = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imvp));

	//_shader->setVec3("Kd", sphere.getKd());
	_shader.setMat4("ModelView", mview);
	_shader.setMat3("NormalMatrix", nmat);

	//_shader.setFloat("Roughness", _sphere.getRoughness());
	//_shader.setFloat("Metallic", _sphere.getMetallic());

	_sphere.draw();
}
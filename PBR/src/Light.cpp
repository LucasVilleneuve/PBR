#include "Light.hh"

Light::Light(int number, glm::vec3 position, glm::vec3 color)
	: _number(number), _sphere(1)
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

void Light::draw(Shader &shader, const glm::mat4 &view)
{
	// Set light properties
	glm::vec3 pos = glm::vec3(view * glm::vec4(_pos, 1.0));
	shader.setVec3("LightPositions[" + std::to_string(_number) + "]", pos);
	shader.setVec3("LightColors[" + std::to_string(_number) + "]", _color);

	//_shader.setFloat("Roughness", _sphere.getRoughness());
	//_shader.setFloat("Metallic", _sphere.getMetallic());

	// Draw sphere
	_sphere.draw(shader, view);
}
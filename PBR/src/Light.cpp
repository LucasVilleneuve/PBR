#include "Light.hh"

Light::Light(int number, Shader &shader, glm::vec3 position, glm::vec3 color)
	: _number(number), _shader(shader), _sphere(shader, 1)
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

	//_shader.setFloat("Roughness", _sphere.getRoughness());
	//_shader.setFloat("Metallic", _sphere.getMetallic());

	// Draw sphere
	_sphere.draw(view);
}
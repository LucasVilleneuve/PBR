#pragma once

#include "Shader.hh"
#include "Sphere.hh"
#include "Model.h"

class Light
{
public:
	Light(int number, glm::vec3 position = glm::vec3(0.0), glm::vec3 color = glm::vec3(300.0));
	~Light() = default;

	Model getModel() const { return _sphere.getModel(); }
	void setModel(Model model) { _sphere.setModel(model); }
	glm::vec3 getPosition() const { return _pos; }
	void setPosition(glm::vec3 pos);
	glm::vec3 getColor() const { return _color; }
	void setColor(glm::vec3 color) { _color = color; }

	void draw(Shader &shader, const glm::mat4 &view);

private:
	Sphere _sphere;
	glm::vec3 _pos;
	glm::vec3 _color;
	int _number;
};
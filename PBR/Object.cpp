#include <glm/gtc/type_ptr.hpp>
#include "Object.hh"

Object::Object()
{
}

Object::~Object()
{
}

glm::vec3 Object::computeNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::vec3 v1 = p2 - p1;
	glm::vec3 v2 = p3 - p1;
	glm::vec3 n = glm::cross(v1, v2);

	return n;
}

bool Object::getVisible() const
{
	return visible;
}

void Object::setVisible(bool v)
{
	visible = v;
}
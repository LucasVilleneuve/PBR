#ifndef _MODEL
#define _MODEL

#include <stack>
#include <glm/glm.hpp>
#include "glm/mat4x4.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Model
{
public:
	glm::mat4 _model;

	Model()
		: _model(1.0)
	{
	}

	Model &glTranslate(float x, float y, float z)
	{
		glm::mat4 Trans = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
		_model = _model * Trans;
		return *this;
	}

	Model &glRotate(float degree, float x, float y, float z)
	{
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(degree), glm::vec3(x, y, z));
		_model = _model * rot;
		return *this;
	}

	Model &glScale(float x, float y, float z)
	{
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
		_model = _model * scale;
		return *this;
	}

	glm::mat4 getMatrix()
	{
		return _model;
	}
};

#endif
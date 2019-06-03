#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>

class Object
{
public:
	Object();
	virtual ~Object();

	virtual void draw() = 0;

	glm::vec3 computeNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

	bool			getVisible() const;
	void			setVisible(bool v);

protected:
	GLuint	vao;
	GLuint	vbo_vertices;
	GLuint	vbo_colors;
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> colors;

	bool		visible = true;

	// Functions
	virtual void	setup() = 0;
};
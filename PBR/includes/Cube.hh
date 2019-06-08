#ifndef __CUBE
#define __CUBE

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include <vector>

class Cube
{
public:

	Cube();
	void setup();
	void draw();
	

	GLuint vaoHandle;
	GLuint vbo_cube_vertices, vbo_cube_colors, ibo_cube_elements;
};

#endif
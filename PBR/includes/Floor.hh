#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

class Floor
{
public:
	Floor(int size, int numcell);
	~Floor() = default;

	void setup(float size, int nSquares);
	void draw();
	void resize(int size, int numcell);

private:
	GLuint _vao;
	GLuint _vboPositions;
	GLuint _vboNormals;

	int size;
	int ncell;
	size_t nbVerts;
};
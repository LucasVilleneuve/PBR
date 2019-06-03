#ifndef ___SPHERE_H
#define ___SPHERE_H

#include "GL/glew.h"
#include <GL/GL.h>
#include <glm/mat4x4.hpp>

#include <vector>

class Sphere2
{
public:
	Sphere2();
	Sphere2(float rad, GLuint sl, GLuint st);
	~Sphere2();

	void draw();
	int getVertexArrayHandle();

	GLuint VAO, VBO_position, VBO_normal, IBO;
	GLuint tex_2d;

private:
	GLuint nVerts, elements;
	float radius;
	GLuint slices, stacks;

	void generateVerts(float *, float *, float *, GLuint *);
};

#endif

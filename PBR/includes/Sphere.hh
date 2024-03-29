#pragma once

#include <vector>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <Shader.hh>
#include "Model.h"

class Sphere
{
public:
	Sphere(float radius, GLuint slices = 64, GLuint stacks = 64);
	~Sphere() = default;

	void draw(Shader &shader, const glm::mat4 &view);

	glm::vec3 getKd() const { return _kd; }
	void setKd(glm::vec3 kd) { _kd = kd; }
	Model getModel() const { return _model; }
	void setModel(Model model) { _model = model; }

protected:
	/* Parameters */
	float _radius;
	GLuint _slices;
	GLuint _stacks;

	Model _model;
	glm::vec3 _kd; // TODO Deprecated

	std::vector<glm::vec3> _positions;
	std::vector<glm::vec3> _normals;
	std::vector<glm::vec2> _uvs;
	std::vector<GLuint> _indices;

	GLuint _vao;
	GLuint _vboPositions;
	GLuint _vboNormals;
	GLuint _ibo;
	//GLuint _tex; // TODO Deprecated

	void generateVertices();

	void setup();
};
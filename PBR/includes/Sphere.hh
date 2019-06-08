#pragma once

#include <vector>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "Model.h"

class Sphere
{
public:
	Sphere(const std::string &textureFileName,
		float radius,
		GLuint slices = 64, GLuint stacks = 64);
	~Sphere() = default;

	void draw();

	glm::vec3 getKd() const { return _kd; }
	void setKd(glm::vec3 kd) { _kd = kd; }
	Model getModel() const { return _model; }
	void setModel(Model model) { _model = model; }
	float getRoughness() const { return _roughness; }
	// TODO Clamp between 0 and 1
	void setRoughness(float roughness) { _roughness = roughness; }
	float getMetallic() const { return _metallic; }
	// TODO Clamp between 0 and 1
	void setMetallic(float metallic) { _metallic = metallic; }

private:
	/* Parameters */
	std::string _texFileName;
	float _radius;
	GLuint _slices;
	GLuint _stacks;

	Model _model;
	glm::vec3 _kd;
	float _roughness;
	float _metallic;

	std::vector<glm::vec3> _positions;
	std::vector<glm::vec3> _normals;
	std::vector<glm::vec2> _uvs;
	std::vector<GLuint> _indices;

	GLuint _vao;
	GLuint _vboPositions;
	GLuint _vboNormals;
	GLuint _ibo;
	GLuint _tex;

	void generateVertices();

	void loadTexture(const std::string &texturePath);
	void setup();
};
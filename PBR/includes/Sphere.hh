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
	void gen();

	glm::vec3 getKd() const { return _kd; }
	void setKd(glm::vec3 kd) { _kd = kd; }
	Model getModel() const { return _model; }
	void setModel(Model model) { _model = model; }
	float getRoughness() const { return _roughness; }
	void setRoughness(float roughness) { _roughness = glm::clamp(roughness, 0.025f, 1.0f); }
	float getMetallic() const { return _metallic; }
	void setMetallic(float metallic) { _metallic = glm::clamp(metallic, 0.0f, 1.0f); }

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
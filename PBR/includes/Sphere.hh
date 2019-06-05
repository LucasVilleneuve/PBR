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
	// TODO Deprecated
	//void setRoughness(float roughness) { _roughness = glm::clamp(roughness, 0.025f, 1.0f); }
	//void setMetallic(float metallic) { _metallic = glm::clamp(metallic, 0.0f, 1.0f); }

	GLuint getAlbedo() const { return _albedo; }
	void setAlbedo(GLuint albedo) { _albedo = albedo; }
	GLuint getRoughness() const { return _roughness; }
	void setRoughness(GLuint roughness) { _roughness = roughness; }
	GLuint getMetallic() const { return _metallic; }
	void setMetallic(GLuint metallic) { _albedo = metallic; }

	// TODO Change that
	static GLuint loadTexture(const std::string &texturePath);

private:
	/* Parameters */
	std::string _texFileName;
	float _radius;
	GLuint _slices;
	GLuint _stacks;

	Model _model;
	glm::vec3 _kd; // TODO Deprecated
	//float _roughness; // TODO Deprecated
	//float _metallic; // TODO Deprecated
	GLuint _albedo;
	GLuint _roughness;
	GLuint _metallic;

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
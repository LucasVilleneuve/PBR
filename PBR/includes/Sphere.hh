#pragma once

#include <vector>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

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

private:
	/* Parameters */
	std::string _texFileName;
	float _radius;
	GLuint _slices;
	GLuint _stacks;

	glm::vec3 _kd;

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
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Stbi/stb_image.h"
#include "TextureLoader.hh"
#include "Sphere.hh"

Sphere::Sphere(Shader &shader, float radius, GLuint slices, GLuint stacks)
	: _shader(shader), _radius(radius), _slices(slices), _stacks(stacks)
{
	GLuint nVerts = (slices + 1) * (stacks + 1);
	GLuint elements = (slices * 2 * (stacks - 1)) * 3;

	// Pre-allocate memory
	_positions.reserve(nVerts * 4);
	_normals.reserve(nVerts * 3);
	_indices.reserve(elements);
	_uvs.reserve(nVerts * 2);

	this->generateVertices();

	this->setup();

	// TODO Move this to Textured SPhere
	//auto &texLoader = TextureLoader::getInstance();
	//_albedo = texLoader.loadTexture("resources/rusted_iron/albedo.png")->id;
	//_metallic = texLoader.loadTexture("resources/rusted_iron/metallic.png")->id;
	//_roughness = texLoader.loadTexture("resources/rusted_iron/roughness.png")->id;
	//_normal = texLoader.loadTexture("resources/rusted_iron/normal.png")->id;
}

void Sphere::generateVertices()
{
	// Generate positions and normals
	GLfloat theta, phi;
	GLfloat thetaFac = glm::two_pi<float>() / _slices;
	GLfloat phiFac = glm::pi<float>() / _stacks;
	GLfloat nx, ny, nz, s, t;

	for (GLuint i = 0; i <= _slices; i++)
	{
		theta = i * thetaFac;
		s = (GLfloat)i / _slices;

		for (GLuint j = 0; j <= _stacks; j++)
		{
			phi = j * phiFac;
			t = (GLfloat)j / _stacks;
			nx = sinf(phi) * cosf(theta);
			ny = sinf(phi) * sinf(theta);
			nz = cosf(phi);

			_positions.push_back(glm::vec4(_radius * nx, _radius * ny, _radius * nz, 1.0));
			_normals.push_back(glm::vec3(nx, ny, nz));
			_uvs.push_back(glm::vec2(s, t));
		}
	}

	// Generate the element list
	for (GLuint i = 0; i < _slices; i++)
	{
		GLuint stackStart = i * (_stacks + 1);
		GLuint nextStackStart = (i + 1) * (_stacks + 1);

		for (GLuint j = 0; j < _stacks; j++)
		{
			if (j == 0)
			{
				_indices.push_back(stackStart);
				_indices.push_back(stackStart + 1);
				_indices.push_back(nextStackStart + 1);
			}
			else if (j == _stacks - 1)
			{
				_indices.push_back(stackStart + j);
				_indices.push_back(stackStart + j + 1);
				_indices.push_back(nextStackStart + j);
			}
			else {
				_indices.push_back(stackStart + j);
				_indices.push_back(stackStart + j + 1);
				_indices.push_back(nextStackStart + j + 1);
				_indices.push_back(nextStackStart + j);
				_indices.push_back(stackStart + j);
				_indices.push_back(nextStackStart + j + 1);
			}
		}
	}
}

void Sphere::setup()
{
	// VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// VBO
	glGenBuffers(1, &_vboPositions);
	glBindBuffer(GL_ARRAY_BUFFER, _vboPositions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _positions.size() * 3, _positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &_vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, _vboNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _normals.size() * 3, _normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	GLuint vbo_tex;
	glGenBuffers(1, &vbo_tex);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _uvs.size() * 2, _uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices.size(), _indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0); // unbinding
}

void Sphere::draw(const glm::mat4 &view)
{
	// Model
	glm::mat4 mview = view * _model.getMatrix();
	glm::mat4 imview = glm::inverse(mview);
	glm::mat3 nmat = glm::mat3(glm::transpose(imview));

	_shader.setMat4("ModelView", mview);
	_shader.setMat3("NormalMatrix", nmat);

	// Activate textures
	//_shader.setInt("AlbedoMap", 0);
	//_shader.setInt("NormalMap", 1);
	//_shader.setInt("MetallicMap", 2);
	//_shader.setInt("RoughnessMap", 3);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, _albedo);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, _normal);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, _metallic);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, _roughness);

	// Draw
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
}
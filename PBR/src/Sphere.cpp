#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Sphere.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "Stbi/stb_image.h"

Sphere::Sphere(const std::string &textureFileName, float radius, GLuint slices, GLuint stacks)
	: _texFileName(textureFileName), _radius(radius), _slices(slices), _stacks(stacks)
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
}

void Sphere::generateVertices()
{
	// Generate positions and normals
	GLfloat theta, phi;
	GLfloat thetaFac = glm::two_pi<float>() / _slices;
	GLfloat phiFac = glm::pi<float>() / _stacks;
	GLfloat nx, ny, nz, s, t;
	GLuint idx = 0, tIdx = 0;

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
			//verts[idx] = _radius * nx;
			//verts[idx + 1] = _radius * ny;
			//verts[idx + 2] = _radius * nz;
			_normals.push_back(glm::vec3(nx, ny, nz));
			//norms[idx] = nx;
			//norms[idx + 1] = ny;
			//norms[idx + 2] = nz;
			idx += 3;

			_uvs.push_back(glm::vec2(s, t));
			//tex[tIdx] = s;
			//tex[tIdx + 1] = t;
			tIdx += 2;
		}
	}

	for (GLuint i = 0; i <= idx; i += 3)
	{
		//_positions.push_back(glm::vec4(verts[i], verts[i + 1], verts[i + 2], 1.0f));
		//_normals.push_back(glm::vec3(norms[i], norms[i + 1], norms[i + 2]));
	}

	// Generate the element list
	idx = 0;
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
				//el[idx] = stackStart;
				//el[idx + 1] = stackStart + 1;
				//el[idx + 2] = nextStackStart + 1;
				idx += 3;
			}
			else if (j == _stacks - 1)
			{
				_indices.push_back(stackStart + j);
				_indices.push_back(stackStart + j + 1);
				_indices.push_back(nextStackStart + j);
				//el[idx] = stackStart + j;
				//el[idx + 1] = stackStart + j + 1;
				//el[idx + 2] = nextStackStart + j;
				idx += 3;
			}
			else {
				_indices.push_back(stackStart + j);
				_indices.push_back(stackStart + j + 1);
				_indices.push_back(nextStackStart + j + 1);
				_indices.push_back(nextStackStart + j);
				_indices.push_back(stackStart + j);
				_indices.push_back(nextStackStart + j + 1);
				//el[idx] = stackStart + j;
				//el[idx + 1] = stackStart + j + 1;
				//el[idx + 2] = nextStackStart + j + 1;
				//el[idx + 3] = nextStackStart + j;
				//el[idx + 4] = stackStart + j;
				//el[idx + 5] = nextStackStart + j + 1;
				idx += 6;
			}
		}
	}

	//for (GLuint i = 0; i <= idx; i++)
	//{
	//	_indices.push_back(el[i]);
	//}
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

	this->loadTexture(_texFileName);

	glBindVertexArray(0); // unbinding
}

void Sphere::loadTexture(const std::string &texturePath)
{
	if (texturePath.empty()) return;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &_tex);

	int width, height, channel;
	unsigned char *data = stbi_load(_texFileName.c_str(), &width, &height, &channel, 0);

	if (data)
	{
		GLenum format;
		if (channel == 1)
			format = GL_RED;
		else if (channel == 3)
			format = GL_RGB;
		else if (channel == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, _tex);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << texturePath.c_str() << std::endl;
	}

	stbi_image_free(data);
}

void Sphere::draw()
{
	// TODO Add uniforms here

	//glUniform3f(shader->uniform("Kd"), 1, 1, 1);
	//glUniform3f(shader->uniform("Ka"), 0.3, 0.3, 0.3);
	//glUniform3f(shader->uniform("Ks"), 0.3, 0.3, 0.3);
	//glUniform1f(shader->uniform("shininess"), 10);

	//glUniform1i(shader->uniform("Tex1"), 0);

	glBindTexture(GL_TEXTURE_2D, _tex);  //activate texture

	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);  //de-activate
}
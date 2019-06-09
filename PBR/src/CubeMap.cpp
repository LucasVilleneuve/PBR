#include <iostream>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Stbi/stb_image.h"
#include "CubeMap.hh"

void CubeMap::setup(unsigned int captureFBO, unsigned int captureRBO)
{
	_captureFBO = captureFBO;
	_captureRBO = captureRBO;
	this->initShaders();
	this->createHdrTexture();
	this->allocateMemoryForFaces();
	this->createProjectionAndViews();
}

void CubeMap::createCubeMap()
{
	// convert HDR equirectangular environment map to cubemap equivalent
	_cubeMapShader->use();
	_cubeMapShader->setInt("equirectangularMap", 0);
	_cubeMapShader->setMat4("Projection", _captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _hdrTexture);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, _captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		_cubeMapShader->setMat4("ModelView", _captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _envCubeMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_cube.draw(); // renders a 1x1 cube
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeMap::preComputeIrradiance()
{
	this->allocateMemoryForIrradiances();
	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	_irradianceShader->use();
	_irradianceShader->setInt("environmentMap", 0);
	_irradianceShader->setMat4("Projection", _captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _envCubeMap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, _captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		_irradianceShader->setMat4("ModelView", _captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_cube.draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CubeMap::draw(glm::mat4 & projection, glm::mat4 & modelView)
{
	_backgroundShader->use();
	_backgroundShader->setMat4("Projection", projection);
	_backgroundShader->setMat4("ModelView", modelView);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _envCubeMap);
	_cube.draw(); // renders a 1x1 cube
}

unsigned int CubeMap::getIrradianceMap()
{
	return (_irradianceMap);
}

void CubeMap::initShaders()
{
	_cubeMapShader = std::make_unique<Shader>("shaders/cubeMap.vert", "shaders/cubeMap.frag");
	_backgroundShader = std::make_unique<Shader>("shaders/background.vert", "shaders/background.frag");
	_irradianceShader = std::make_unique<Shader>("shaders/cubeMap.vert", "shaders/irradiance.frag");

	_cubeMapShader->addUniform("equirectangularMap");
	_cubeMapShader->addUniform("Projection");
	_cubeMapShader->addUniform("ModelView");

	_backgroundShader->addUniform("environmentMap");
	_backgroundShader->setInt("environmentMap", 0);
	_backgroundShader->addUniform("Projection");
	_backgroundShader->addUniform("ModelView");
	_irradianceShader->addUniform("environmentMap");
	_irradianceShader->addUniform("Projection");
	_irradianceShader->addUniform("ModelView");
}

void CubeMap::createHdrTexture()
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrComponents;
	float *data = stbi_loadf("resources/newport_loft.hdr", &width, &height, &nrComponents, 0);

	if (data)
	{
		glGenTextures(1, &_hdrTexture);
		glBindTexture(GL_TEXTURE_2D, _hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}
}

void CubeMap::allocateMemoryForFaces()
{
	glGenTextures(1, &_envCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _envCubeMap);

	for (unsigned int index = 0; index < 6; ++index)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void CubeMap::allocateMemoryForIrradiances()
{
	glGenTextures(1, &_irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, _captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, _captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
}

void CubeMap::createProjectionAndViews()
{
	_captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	_captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	_captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	_captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	_captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	_captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

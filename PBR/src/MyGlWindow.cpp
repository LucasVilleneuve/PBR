#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL true
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MyGlWindow.hh"

MyGlWindow::MyGlWindow(int width, int height)
	: _width(width), _height(height)
{
	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (width / (float)height);
	_viewer = std::make_unique<Viewer>(viewPoint, viewCenter, upVector, 45.0f, aspect);
	_fbo = std::make_unique<FBO>();
	_cubeMap = std::make_unique<CubeMap>();

	this->init();
}

MyGlWindow::~MyGlWindow() = default;

void MyGlWindow::init()
{
	_pbrType = PBR_TYPE::DIRECT_LIGHT;
	_pbrType = PBR_TYPE::IBL_DIFFUSE_IRRADIANCE;

	_shader = std::make_unique<Shader>("shaders/pbr.vert", "shaders/pbr.frag");
	_shaderibl = std::make_unique<Shader>("shaders/pbr.vert", "shaders/pbrDiffuseIrradiance.frag");

	_lights.push_back(Light(0, glm::vec3(0, 0, 20), lightColor));
	_lights.push_back(Light(1, glm::vec3(7.5, 7.5, 20), lightColor));
	_lights.push_back(Light(2, glm::vec3(7.5, 0, 20), lightColor));
	_lights.push_back(Light(3, glm::vec3(0, 7.5, 20), lightColor));
	_wall = std::make_unique<SphereWall>();

	initShader(*_shader);
	initShader(*_shaderibl);

	_cubeMap->setup(_fbo->getFBO(), _fbo->getRBO());
	_cubeMap->createCubeMap();
	_cubeMap->preComputeIrradiance();

	if (_pbrType == PBR_TYPE::IBL_DIFFUSE_IRRADIANCE)
	{
		_currentShader = _shaderibl.get();
	}
	else
	{
		_currentShader = _shader.get();
	}
}

void MyGlWindow::initShader(Shader &shader)
{
	shader.use();

	shader.addUniform("MetallicMap");
	shader.addUniform("RoughnessMap");
	shader.addUniform("AlbedoMap");
	shader.addUniform("NormalMap");
	shader.addUniform("Roughness");
	shader.addUniform("Metallic");
	shader.addUniform("Projection");
	shader.addUniform("Model");
	shader.addUniform("NormalMatrix");
	shader.addUniform("ModelView");
	shader.addUniform("CamPos");
	shader.addUniform("GammaCorr");
	shader.addUniform("ValuesFromFile");
	shader.addUniform("irradianceMap");

	for (unsigned int i = 0; i < _lights.size(); ++i)
	{
		shader.addUniform("LightPositions[" + std::to_string(i) + "]");
		shader.addUniform("LightColors[" + std::to_string(i) + "]");
	}

	shader.disable();
}

void MyGlWindow::resize(int width, int height)
{
	_width = width;
	_height = height;
}

void MyGlWindow::setAspect(float aspectRatio)
{
	_viewer->setAspectRatio(aspectRatio);
}

void MyGlWindow::switchPbrType(bool ibl)
{
	if (ibl)
	{
		_currentShader = _shaderibl.get();
	}
	else
	{
		_currentShader = _shader.get();
	}
}

void MyGlWindow::draw()
{
	glClearColor(bkgColor[0], bkgColor[1], bkgColor[2], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, _width, _height);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_2D);

	glm::vec3 eye = _viewer->getViewPoint();
	glm::vec3 look = _viewer->getViewCenter();
	glm::vec3 up = _viewer->getUpVector();
	glm::mat4 view = glm::lookAt(eye, look, up);
	glm::mat4 projection = glm::perspective(45.0f, (float)_width / (float)_height, 0.1f, 500.0f);

	switchPbrType(ibl);

	_currentShader->use();
	_currentShader->setMat4("Projection", projection);
	_currentShader->setVec3("CamPos", glm::mat3(view) * eye); // TODO Should we multiply by the view matrix ?

	auto addTextures = [this]() {
		if (_pbrType == PBR_TYPE::IBL_DIFFUSE_IRRADIANCE)
		{
			if (valuesFromFile)
			{
				_currentShader->setInt("irradianceMap", 4);
				glActiveTexture(GL_TEXTURE4);
			}
			else
			{
				_currentShader->setInt("irradianceMap", 0);
				glActiveTexture(GL_TEXTURE0);
			}
			glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeMap->getIrradianceMap());
		}
	};

	_wall->draw(*_currentShader, view, addTextures);

	for (Light &light : _lights)
	{
		light.setColor(lightColor);
		light.draw(*_currentShader, view);
	}

	_currentShader->setBool("GammaCorr", gammaCorr);
	_currentShader->setBool("ValuesFromFile", valuesFromFile);

	_currentShader->disable();

	glDepthFunc(GL_LEQUAL);

	_cubeMap->draw(projection, view);
}
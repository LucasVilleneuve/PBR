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
	_cubeMap = std::make_unique<CubeMap>();

	this->init();
}

MyGlWindow::~MyGlWindow() = default;

void MyGlWindow::init()
{
	_shader = std::make_unique<Shader>("shaders/pbr.vert", "shaders/pbr.frag");

	_wall = std::make_unique<SphereWall>(*_shader);

	_lights.push_back(Light(0, *_shader, glm::vec3(0, 0, 20), lightColor));
	_lights.push_back(Light(1, *_shader, glm::vec3(7.5, 7.5, 20), lightColor));
	_lights.push_back(Light(2, *_shader, glm::vec3(7.5, 0, 20), lightColor));
	_lights.push_back(Light(3, *_shader, glm::vec3(0, 7.5, 20), lightColor));

	_shader->use();
	_shader->addUniform("MetallicMap");
	_shader->addUniform("RoughnessMap");
	_shader->addUniform("AlbedoMap");
	_shader->addUniform("NormalMap");
	_shader->addUniform("Roughness");
	_shader->addUniform("Metallic");
	//_shader->addUniform("Kd");
	//_shader->addUniform("Ka");
	//_shader->addUniform("Ks");
	//_shader->addUniform("shininess");
	_shader->addUniform("Projection");
	//_shader->addUniform("View");
	_shader->addUniform("Model");
	_shader->addUniform("NormalMatrix");
	_shader->addUniform("ModelView");
	_shader->addUniform("CamPos");

	_shader->addUniform("GammaCorr");

	for (int i = 0; i < _lights.size(); ++i)
	{
		_shader->addUniform("LightPositions[" + std::to_string(i) + "]");
		_shader->addUniform("LightColors[" + std::to_string(i) + "]");
	}

	//_shader->setVec3("Ka", glm::vec3(0.3, 0.3, 0.3));
	//_shader->setVec3("Ks", glm::vec3(1.0, 1.0, 1.0));
	//_shader->setFloat("shininess", 12.0);
	_shader->disable();
	_cubeMap->drawOnFBO();
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

	glm::mat4 mview;
	glm::mat4 imvp;
	glm::mat3 nmat;

	_shader->use();
	_shader->setMat4("Projection", projection);
	_shader->setVec3("CamPos", glm::mat3(view) * eye); // TODO Should we multiply by the view matrix ?

	_wall->draw(view);

	for (Light &light : _lights)
	{
		light.setColor(lightColor);
		light.draw(view);
	}

	_shader->setBool("GammaCorr", gammaCorr);

	_shader->disable();

	glDisable(GL_CULL_FACE);

	_cubeMap->draw(projection, view);
}

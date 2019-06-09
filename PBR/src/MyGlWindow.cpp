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
	/*_pbrType = PBR_TYPE::IBL_DIFFUSE_IRRADIANCE;*/

	if (_pbrType == PBR_TYPE::DIRECT_LIGHT)
		_shader = std::make_unique<Shader>("shaders/pbr.vert", "shaders/pbr.frag");
	else if (_pbrType == PBR_TYPE::IBL_DIFFUSE_IRRADIANCE)
		_shader = std::make_unique<Shader>("shaders/pbr.vert", "shaders/pbrDiffuseIrradiance.frag");

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
	_shader->addUniform("ValuesFromFile");

	for (int i = 0; i < _lights.size(); ++i)
	{
		_shader->addUniform("LightPositions[" + std::to_string(i) + "]");
		_shader->addUniform("LightColors[" + std::to_string(i) + "]");
	}

	//_shader->setVec3("Ka", glm::vec3(0.3, 0.3, 0.3));
	//_shader->setVec3("Ks", glm::vec3(1.0, 1.0, 1.0));
	//_shader->setFloat("shininess", 12.0);
	_shader->disable();
	_cubeMap->setup(_fbo->getFBO(), _fbo->getRBO());
	_cubeMap->createCubeMap();
	_cubeMap->preComputeIrradiance();
	if (_pbrType == PBR_TYPE::IBL_DIFFUSE_IRRADIANCE)
	{
		_shader->addUniform("irradianceMap");
		_shader->setInt("irradianceMap", 4);
	}
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

	auto addTextures = [this]() {
		if (_pbrType == PBR_TYPE::IBL_DIFFUSE_IRRADIANCE)
		{
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeMap->getIrradianceMap());
		}
	};

	_wall->draw(view, addTextures);

	for (Light &light : _lights)
	{
		light.setColor(lightColor);
		light.draw(view);
	}

	_shader->setBool("GammaCorr", gammaCorr);
	_shader->setBool("ValuesFromFile", valuesFromFile);

	_shader->disable();

	glDepthFunc(GL_LEQUAL);

	_cubeMap->draw(projection, view);
}
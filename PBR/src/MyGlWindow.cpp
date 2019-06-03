#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL true
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MyGlWindow.hh"

MyGlWindow::MyGlWindow(int width, int height)
	: _width(width), _height(height), _light("", 1)
{
	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (width / (float)height);
	_viewer = std::make_unique<Viewer>(viewPoint, viewCenter, upVector, 45.0f, aspect);

	this->init();
}

MyGlWindow::~MyGlWindow()
{
}

void MyGlWindow::init()
{
	_shader = std::make_unique<Shader>("shaders/phong.vert", "shaders/phong.frag");

	unsigned int numberOfSpheres = 3;
	Sphere basicSphere("", 1);
	basicSphere.setKd(glm::vec3(0.8, 0.2, 0.2));
	_light.setKd(glm::vec3(1.0));

	for (int i = 0; i < numberOfSpheres; ++i)
	{
		_spheres.push_back(basicSphere);
	}

	_shader->use();
	_shader->addUniform("LightPosition");
	_shader->addUniform("LightColor");
	_shader->addUniform("Kd");
	_shader->addUniform("Ka");
	_shader->addUniform("Ks");
	_shader->addUniform("shininess");
	_shader->addUniform("Projection");
	_shader->addUniform("View");
	_shader->addUniform("Model");
	_shader->addUniform("NormalMatrix");
	_shader->addUniform("ModelView");

	_shader->setVec3("LightPosition", lightPosition);
	_shader->setVec3("LightColor", glm::vec3(0.8, 0.8, 0.8));
	_shader->setVec3("Ka", glm::vec3(0.3, 0.3, 0.3));
	_shader->setVec3("Ks", glm::vec3(1.0, 1.0, 1.0));
	_shader->setFloat("shininess", 12.0);
	_shader->disable();
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

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::vec3 eye = _viewer->getViewPoint();
	glm::vec3 look = _viewer->getViewCenter();
	glm::vec3 up = _viewer->getUpVector();
	glm::mat4 view = glm::lookAt(eye, look, up);
	glm::mat4 projection = glm::perspective(45.0f, (float)_width / (float)_height, 0.1f, 500.0f);

	Model m;
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mview;
	glm::mat4 imvp;
	glm::mat3 nmat;

	_shader->use();
	_shader->setMat4("Projection", projection);
	_shader->setVec3("LightPosition", glm::vec3(view * glm::vec4(lightPosition, 1.0)));

	float xOffset = 0;
	for (auto sphere : _spheres)
	{
		m.glPushMatrix();
		m.glTranslate(xOffset, 0, 0);
		model = m.getMatrix();
		m.glPopMatrix();

		mview = view * model;
		imvp = glm::inverse(mview);
		nmat = glm::mat3(glm::transpose(imvp));

		_shader->setVec3("Kd", sphere.getKd());
		_shader->setMat4("ModelView", mview);
		_shader->setMat3("NormalMatrix", nmat);

		sphere.draw();

		xOffset += 2.5;
	}

	m.glPushMatrix();
	m.glTranslate(lightPosition.x, lightPosition.y, lightPosition.z);
	model = m.getMatrix();
	m.glPopMatrix();

	mview = view * model;
	imvp = glm::inverse(mview);
	nmat = glm::mat3(glm::transpose(imvp));
	_shader->setMat4("ModelView", mview);
	_shader->setMat3("NormalMatrix", nmat);
	_shader->setVec3("Kd", _light.getKd());

	_light.draw();

	_shader->disable();
}
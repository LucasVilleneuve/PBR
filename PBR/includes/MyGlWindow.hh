#pragma once

#include <memory>
#include <vector>
#include "Shader.hh"
#include "Viewer/Viewer.h"
#include "SphereWall.hh"
#include "Light.hh"
#include "Model.h"

class MyGlWindow
{
public:
	MyGlWindow(int width, int height);
	~MyGlWindow();

	void draw();
	void resize(int width, int height);
	void setAspect(float aspectRatio);

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	Viewer *getViewer() const { return _viewer.get(); }

private:
	int _width;
	int _height;
	std::unique_ptr<Viewer> _viewer;
	std::unique_ptr<Shader> _shader;
	float bkgColor[3] = { 0.2f, 0.2f, 0.2f };

public:
	// TODO Change to private
	glm::vec3 lightPosition = glm::vec3(12.5, 12.5, -30); // TODO Deprecated
	glm::vec3 lightColor = glm::vec3(1000.0f);
	bool gammaCorr = false;
private:
	std::unique_ptr<SphereWall> _wall;
	std::vector<Light> _lights;

	const float DEFAULT_VIEW_POINT[3] = { 12.5, 15, -20 };
	const float DEFAULT_VIEW_CENTER[3] = { 12.5, 12.5, 0 };
	const float DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

	void init();
};
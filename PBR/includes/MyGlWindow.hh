#pragma once

#include <memory>
#include <vector>
#include "Shader.hh"
#include "Viewer/Viewer.h"
#include "SphereWall.hh"
#include "Light.hh"
#include "Model.h"
#include "FBO.hh"
#include "CubeMap.hh"

class MyGlWindow
{
public:
	MyGlWindow(int width, int height);
	~MyGlWindow();

	void draw();
	void resize(int width, int height);
	void setAspect(float aspectRatio);
	void switchPbrType(bool ibl);

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	Viewer *getViewer() const { return _viewer.get(); }

	enum class PBR_TYPE
	{
		DIRECT_LIGHT,
		IBL_DIFFUSE_IRRADIANCE
	};

private:
	int _width;
	int _height;
	std::unique_ptr<Viewer>		_viewer;
	std::unique_ptr<Shader>		_shader;
	std::unique_ptr<Shader>		_shaderibl;
	Shader						*_currentShader = nullptr;
	std::unique_ptr<FBO>		_fbo;
	std::unique_ptr<CubeMap>	_cubeMap;
	PBR_TYPE					_pbrType;
	float bkgColor[3] = { 0.2f, 0.2f, 0.2f };

public:
	// TODO Change to private
	glm::vec3 lightColor = glm::vec3(1000.0f);
	bool gammaCorr = false;
	bool valuesFromFile = true;
	bool ibl = false;

private:
	std::unique_ptr<SphereWall> _wall;
	std::vector<Light> _lights;

	const float DEFAULT_VIEW_POINT[3] = { 0, 0, 5 };
	const float DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
	const float DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

	void init();
	void initShader(Shader &shader);
};
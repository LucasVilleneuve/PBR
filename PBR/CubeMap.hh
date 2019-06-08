#include <memory>
#include "Shader.hh"
#include "Cube.hh"

class CubeMap
{

public:

	CubeMap();
	~CubeMap() = default;

	void drawOnFBO();
	void draw(glm::mat4 &projection, glm::mat4 &modelView);

private:

	void initFBO();
	void createHdrTexture();
	void allocateMemoryForFaces();

	unsigned int _hdrTexture;
	unsigned int _captureFBO;
	unsigned int _captureRBO;
	unsigned int _envCubeMap;

	std::unique_ptr<Shader> _shader;
	std::unique_ptr<Shader> _backgroundShader;
	Cube					_cube;


};


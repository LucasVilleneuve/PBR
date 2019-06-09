#include <memory>
#include "Shader.hh"
#include "Cube.hh"

class CubeMap
{

public:

	CubeMap() = default;
	~CubeMap() = default;

	void setup(unsigned int captureFBO, unsigned int captureRBO);
	void createCubeMap();
	void preComputeIrradiance();
	void draw(glm::mat4 &projection, glm::mat4 &modelView);

	unsigned int getIrradianceMap();

private:

	void initShaders();
	void createHdrTexture();
	void allocateMemoryForFaces();
	void allocateMemoryForIrradiances();
	void createProjectionAndViews();

	unsigned int	_captureFBO;
	unsigned int	_captureRBO;

	glm::mat4		_captureProjection;
	glm::mat4		_captureViews[6];
	unsigned int	_hdrTexture;
	unsigned int	_envCubeMap;
	unsigned int	_irradianceMap;

	std::unique_ptr<Shader> _cubeMapShader;
	std::unique_ptr<Shader> _irradianceShader;
	std::unique_ptr<Shader> _backgroundShader;
	Cube					_cube;


};


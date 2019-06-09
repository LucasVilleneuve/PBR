#include <memory>
#include "Shader.hh"

class FBO
{

public:

	FBO();
	~FBO() = default;

	unsigned int getFBO();
	unsigned int getRBO();

private:

	void initFBO();

	unsigned int _captureFBO;
	unsigned int _captureRBO;
};


#include "FBO.hh"

FBO::FBO()
{
	this->initFBO();
}

unsigned int FBO::getFBO()
{
	return (_captureFBO);
}

unsigned int FBO::getRBO()
{
	return (_captureRBO);
}

void FBO::initFBO()
{
	glGenFramebuffers(1, &_captureFBO);
	glGenRenderbuffers(1, &_captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, _captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, _captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _captureRBO);
}


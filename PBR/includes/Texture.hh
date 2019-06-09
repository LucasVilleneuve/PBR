#pragma once

#include <GLFW/glfw3.h>
#include <string>

struct Texture
{
public:
	~Texture() = default;

	/* Parameters */
	GLuint id;
	std::string path;
	unsigned char *data; // TODO Deprecated / Not used
	int width;
	int height;
	int channel;
};
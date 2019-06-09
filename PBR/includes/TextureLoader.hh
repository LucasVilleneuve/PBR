#pragma once

#include <vector>
#include "Texture.hh"

class TextureLoader
{
public:
	Texture *loadTexture(const std::string &texturePath);
	Texture *getTexture(const std::string &texturePath);

	GLenum getFormat(int channel);

private:
	/* Parameters */
	std::vector<Texture> _textures;

	TextureLoader() = default;

	void addTexture(const GLuint &id, const std::string &path, int width, int height, int channel);

public:
	TextureLoader(TextureLoader const&) = delete;
	void operator=(TextureLoader const&) = delete;

	static TextureLoader& getInstance()
	{
		static TextureLoader instance;
		return instance;
	}
};
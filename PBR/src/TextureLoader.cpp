#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include "Stbi/stb_image.h"
#include "TextureLoader.hh"

Texture *TextureLoader::loadTexture(const std::string &texturePath)
{
	Texture *texture = getTexture(texturePath);

	if (texture) // If the texture has already been loaded
	{
		return texture;
	}
	else
	{
		std::cout << "Loading " << texturePath << std::endl;
		GLuint texId;
		glGenTextures(1, &texId);

		int width, height, channel;
		unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &channel, 0);

		std::cout << "Loaded! Null ? ";
		std::cout << ((data == nullptr) ? "Yes" : "No") << std::endl;

		if (data)
		{
			GLenum format = getFormat(channel);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texId);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
				GL_UNSIGNED_BYTE, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);

			addTexture(texId, texturePath, width, height, channel);

			stbi_image_free(data);

			return getTexture(texturePath);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << texturePath.c_str() << std::endl;
			return nullptr;
		}
	}
}

void TextureLoader::addTexture(const GLuint &texId, const std::string & texturePath, int width, int height, int channel)
{
	Texture newTexture;
	newTexture.id = texId;
	newTexture.path = texturePath;
	newTexture.width = width;
	newTexture.height = height;
	newTexture.channel = channel;

	_textures.push_back(newTexture);
}

Texture *TextureLoader::getTexture(const std::string &texturePath)
{
	auto it = std::find_if(_textures.begin(), _textures.end(),
		[texturePath](const Texture &texture) {
		return texture.path == texturePath;
	});

	// Returns nullptr if no textures has been found
	return (it == _textures.end()) ? nullptr : &(*it);
}

GLenum TextureLoader::getFormat(int channel)
{
	if (channel == 1)
		return GL_RED;
	else if (channel == 3)
		return GL_RGB;
	else if (channel == 4)
		return GL_RGBA;

	throw std::runtime_error("Incorrect channel");
}
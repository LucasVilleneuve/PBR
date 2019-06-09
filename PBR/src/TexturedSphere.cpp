#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Stbi/stb_image.h"
#include "TextureLoader.hh"
#include "TexturedSphere.hh"

TexturedSphere::TexturedSphere(const std::string &texturesPath, Shader &shader, float radius, GLuint slices, GLuint stacks)
	: Sphere(shader, radius, slices, stacks)
{
	auto &texLoader = TextureLoader::getInstance();
	_albedo = texLoader.loadTexture(texturesPath + "/albedo.png")->id;
	_metallic = texLoader.loadTexture(texturesPath + "/metallic.png")->id;
	_roughness = texLoader.loadTexture(texturesPath + "/roughness.png")->id;
	_normal = texLoader.loadTexture(texturesPath + "/normal.png")->id;
}

void TexturedSphere::draw(const glm::mat4 &view)
{
	// Activate textures
	_shader.setInt("AlbedoMap", 0);
	_shader.setInt("NormalMap", 1);
	_shader.setInt("MetallicMap", 2);
	_shader.setInt("RoughnessMap", 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _albedo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _metallic);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _roughness);

	Sphere::draw(view);
}
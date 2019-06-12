#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Stbi/stb_image.h"
#include "TextureLoader.hh"
#include "TexturedSphere.hh"

TexturedSphere::TexturedSphere(const std::string &texturesPath, float radius, GLuint slices, GLuint stacks)
	: Sphere(radius, slices, stacks)
{
	auto &texLoader = TextureLoader::getInstance();
	_albedo = texLoader.loadTexture(texturesPath + "/albedo.png")->id;
	_metallic = texLoader.loadTexture(texturesPath + "/metallic.png")->id;
	_roughness = texLoader.loadTexture(texturesPath + "/roughness.png")->id;
	_normal = texLoader.loadTexture(texturesPath + "/normal.png")->id;
}

void TexturedSphere::draw(Shader &shader, const glm::mat4 &view)
{
	// Activate textures
	shader.setInt("AlbedoMap", 0);
	shader.setInt("NormalMap", 1);
	shader.setInt("MetallicMap", 2);
	shader.setInt("RoughnessMap", 3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _albedo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _metallic);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _roughness);

	Sphere::draw(shader, view);
}
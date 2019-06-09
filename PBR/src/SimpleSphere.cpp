#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Stbi/stb_image.h"
#include "TextureLoader.hh"
#include "SimpleSphere.hh"

SimpleSphere::SimpleSphere(Shader &shader, float radius, GLuint slices, GLuint stacks)
	: Sphere(shader, radius, slices, stacks)
{
}

SimpleSphere::SimpleSphere(Shader &shader, float roughness, float metallic,
	float radius, GLuint slices, GLuint stacks)
	: Sphere(shader, radius, slices, stacks)
{
	this->setMetallic(metallic);
	this->setRoughness(roughness);
}

void SimpleSphere::draw(const glm::mat4 &view)
{
	_shader.setFloat("Roughness", _roughness);
	_shader.setFloat("Metallic", _metallic);

	Sphere::draw(view);
}
#include <glm/gtc/constants.hpp>
#include <GL/glew.h>
#include <iostream>
#include "Stbi/stb_image.h"
#include "TextureLoader.hh"
#include "SimpleSphere.hh"

SimpleSphere::SimpleSphere(float radius, GLuint slices, GLuint stacks)
	: Sphere(radius, slices, stacks)
{
}

SimpleSphere::SimpleSphere(float roughness, float metallic,
	float radius, GLuint slices, GLuint stacks)
	: Sphere(radius, slices, stacks)
{
	this->setMetallic(metallic);
	this->setRoughness(roughness);
}

void SimpleSphere::draw(Shader &shader, const glm::mat4 &view)
{
	shader.setFloat("Roughness", _roughness);
	shader.setFloat("Metallic", _metallic);

	Sphere::draw(shader, view);
}
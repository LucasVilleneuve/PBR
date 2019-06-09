#pragma once

#include "Sphere.hh"

class SimpleSphere : public Sphere
{
public:
	SimpleSphere(Shader &shader,
		float radius, GLuint slices = 64, GLuint stacks = 64);
	SimpleSphere(Shader &shader, float roughness, float metallic,
		float radius, GLuint slices = 64, GLuint stacks = 64);
	~SimpleSphere() = default;

	void draw(const glm::mat4 &view);

	void setRoughness(float roughness) { _roughness = glm::clamp(roughness, 0.025f, 1.0f); }
	void setMetallic(float metallic) { _metallic = glm::clamp(metallic, 0.0f, 1.0f); }
	float getRoughness() const { return _roughness; }
	float getMetallic() const { return _metallic; }

private:
	/* Parameters */
	float _roughness;
	float _metallic;
};
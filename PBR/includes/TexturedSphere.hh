#pragma once

#include "Sphere.hh"

class TexturedSphere : public Sphere
{
public:
	TexturedSphere(const std::string &texturesPath, Shader &shader,
		float radius, GLuint slices = 64, GLuint stacks = 64);
	~TexturedSphere() = default;

	void draw(const glm::mat4 &view);

	// TODO Deprecated
	//void setRoughness(float roughness) { _roughness = glm::clamp(roughness, 0.025f, 1.0f); }
	//void setMetallic(float metallic) { _metallic = glm::clamp(metallic, 0.0f, 1.0f); }

	GLuint getAlbedo() const { return _albedo; }
	void setAlbedo(GLuint albedo) { _albedo = albedo; }
	GLuint getRoughness() const { return _roughness; }
	void setRoughness(GLuint roughness) { _roughness = roughness; }
	GLuint getMetallic() const { return _metallic; }
	void setMetallic(GLuint metallic) { _metallic = metallic; }
	GLuint getNormal() const { return _normal; }
	void setNormal(GLuint normal) { _normal = normal; }

private:
	/* Parameters */
	//float _roughness; // TODO Deprecated
	//float _metallic; // TODO Deprecated
	GLuint _albedo;
	GLuint _roughness;
	GLuint _metallic;
	GLuint _normal;
};
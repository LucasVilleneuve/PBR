#pragma once

#include "Sphere.hh"

class TexturedSphere : public Sphere
{
public:
	TexturedSphere(const std::string &texturesPath,
		float radius, GLuint slices = 64, GLuint stacks = 64);
	~TexturedSphere() = default;

	void draw(Shader &shader, const glm::mat4 &view);

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
	GLuint _albedo;
	GLuint _roughness;
	GLuint _metallic;
	GLuint _normal;
};
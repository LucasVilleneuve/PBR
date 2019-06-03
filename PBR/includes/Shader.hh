#pragma once

#include <string>
#include <map>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

class Shader
{
public:
	Shader(std::string vertexShaderFilename, std::string fragmentShaderFilename);
	~Shader();

	// Method to initialise a shader program from shaders provided as files
	void initFromFiles(std::string vertexShaderFilename, std::string fragmentShaderFilename);

	// Method to enable the shader program - we'll suggest this for inlining
	inline void use()
	{
		// Santity check that we're initialised and ready to go...
		if (initialised)
		{
			glUseProgram(programId);
		}
		else
		{
			std::string msg = "Shader program " + programId;
			msg += " not initialised - aborting.";
			throw std::runtime_error(msg);
		}
	}

	// Method to disable the shader - we'll also suggest this for inlining
	inline void disable()
	{
		glUseProgram(0);
	}

	// Method to add a uniform to the shader and return the bound location
	int addUniform(const std::string uniformName);

	// Method to returns the bound location of a named uniform
	GLuint uniform(const std::string uniformName) const;

	/* Utilities */
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec2(const std::string &name, float x, float y) const;
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setVec4(const std::string &name, float x, float y, float z, float w);
	void setMat2(const std::string &name, const glm::mat2 &mat) const;
	void setMat3(const std::string &name, const glm::mat3 &mat) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
	// static DEBUG flag - if set to false then, errors aside, we'll run completely silent
	static const bool DEBUG = true;

	// We'll use an enum to differentiate between shaders and shader programs when querying the info log
	enum class ObjectType
	{
		SHADER, PROGRAM
	};

	// Shader program and individual shader Ids
	GLuint programId;
	GLuint vertexShaderId;
	GLuint fragmentShaderId;

	// How many shaders are attached to the shader program
	GLuint shaderCount;

	// Map of uniforms and their binding locations
	std::map<std::string, int> uniformMap;

	// Has this shader program been initialised?
	bool initialised;

	/* Private methods */

	/* Private method to compile/attach/link/verify the shaders.
		Note: Rather than returning a boolean as a success/fail status we'll just consider
		 a failure here to be an unrecoverable error and throw a runtime_error. */
	void initialise(std::string vertexShaderSource, std::string fragmentShaderSource);

	// Private method to load the shader source code from a file
	std::string loadShaderFromFile(const std::string filename);

	// Private method to return the current shader program info log as a string
	std::string getInfoLog(ObjectType type, int id);

	// Private method to compile a shader of a given type
	GLuint compileShader(std::string shaderSource, GLenum shaderType);
};
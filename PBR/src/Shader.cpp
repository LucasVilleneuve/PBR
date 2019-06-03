#include <iostream>
#include <fstream>
#include <sstream>
#include "Shader.hh"

Shader::Shader(std::string vertexShaderFilename, std::string fragmentShaderFilename)
{
	/* We start in a non-initialised state - calling initFromFiles() will
	initialise the shader. */
	initialised = false;

	// Generate a unique Id / handle for the shader program
	// Note: We MUST have a valid rendering context before generating the programId or we'll segfault!
	programId = glCreateProgram();
	glUseProgram(programId);

	// Initially, we have zero shaders attached to the program
	shaderCount = 0;

	this->initFromFiles(vertexShaderFilename, fragmentShaderFilename);
}

Shader::~Shader()
{
	/* Delete the shader program from the graphics card memory to
	free all the resources it's been using */
	glDeleteProgram(programId);
}

void Shader::initFromFiles(std::string vertexShaderFilename, std::string fragmentShaderFilename)
{
	// Get the shader file contents as strings
	std::string vertexShaderSource = loadShaderFromFile(vertexShaderFilename);
	std::string fragmentShaderSource = loadShaderFromFile(fragmentShaderFilename);

	initialise(vertexShaderSource, fragmentShaderSource);
}

int Shader::addUniform(const std::string uniformName)
{
	// Add the uniform location value for the uniformName key
	uniformMap[uniformName] = glGetUniformLocation(programId, uniformName.c_str());

	// Check to ensure that the shader contains a uniform with this name
	if (uniformMap[uniformName] == -1)
	{
		std::cout << "Could not add uniform: " << uniformName << " - location returned -1." << std::endl;
	}
	else // Valid uniform location? Inform user if we're in debug mode.
	{
		if (DEBUG)
		{
			std::cout << "Uniform " << uniformName << " bound to location: " << uniformMap[uniformName] << std::endl;
		}
	}

	// Return the uniform location
	return uniformMap[uniformName];
}

GLuint Shader::uniform(const std::string uniformName) const
{
	// Create an iterator to look through our uniform map (only create iterator on first run -
	// reuse it for all further calls).
	static std::map<std::string, int>::const_iterator uniformIter;

	// Try to find the named uniform
	uniformIter = uniformMap.find(uniformName);

	// Found it? Great - pass it back! Didn't find it? Alert user and halt.
	if (uniformIter == uniformMap.end())
	{
		std::cout << "Could not find uniform in shader program: " << uniformName << std::endl;
		return 0;
	}

	// Otherwise return the attribute location from the uniform map
	return uniformMap.at(uniformName);
}

void Shader::initialise(std::string vertexShaderSource, std::string fragmentShaderSource)
{
	// Compile the shaders and return their id values
	vertexShaderId = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
	fragmentShaderId = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	// Attach the compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	// Link the shader program - details are placed in the program info log
	glLinkProgram(programId);

	// Once the shader program has the shaders attached and linked, the shaders are no longer required.
	// If the linking failed, then we're going to abort anyway so we still detach the shaders.
	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);

	// Check the program link status and throw a runtime_error if program linkage failed.
	GLint programLinkSuccess = GL_FALSE;
	glGetProgramiv(programId, GL_LINK_STATUS, &programLinkSuccess);
	if (programLinkSuccess == GL_TRUE)
	{
		if (DEBUG)
		{
			std::cout << "Shader program link successful." << std::endl;
		}
	}
	else
	{
		std::cout << "Shader program link failed: " << getInfoLog(ObjectType::PROGRAM, programId) << std::endl;
	}

	// Validate the shader program
	glValidateProgram(programId);

	// Check the validation status and throw a runtime_error if program validation failed
	GLint programValidatationStatus;
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &programValidatationStatus);
	if (programValidatationStatus == GL_TRUE)
	{
		if (DEBUG)
		{
			std::cout << "Shader program validation successful." << std::endl;
		}
	}
	else
	{
		std::cout << "Shader program validation failed: " << getInfoLog(ObjectType::PROGRAM, programId) << std::endl;
	}

	// Finally, the shader program is initialised
	initialised = true;
}

std::string Shader::loadShaderFromFile(const std::string filename)
{
	// Create an input filestream and attempt to open the specified file
	std::ifstream file(filename.c_str());

	// If we couldn't open the file we'll bail out
	if (!file.good())
	{
		std::string errStr = "Failed to open file: " + filename;
		throw std::runtime_error(errStr);
	}

	// Otherwise, create a string stream...
	std::stringstream stream;

	// ...and dump the contents of the file into it.
	stream << file.rdbuf();

	// Now that we've read the file we can close it
	file.close();

	// Finally, convert the stringstream into a string and return it
	return stream.str();
}

std::string Shader::getInfoLog(ObjectType type, int id)
{
	GLint infoLogLength;
	if (type == ObjectType::SHADER)
	{
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	}
	else // type must be ObjectType::PROGRAM
	{
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	}

	GLchar *infoLog = new GLchar[infoLogLength + 1];
	if (type == ObjectType::SHADER)
	{
		glGetShaderInfoLog(id, infoLogLength, NULL, infoLog);
	}
	else // type must be ObjectType::PROGRAM
	{
		glGetProgramInfoLog(id, infoLogLength, NULL, infoLog);
	}

	// Convert the info log to a string
	std::string infoLogString(infoLog);

	// Delete the char array version of the log
	delete[] infoLog;

	// Finally, return the string version of the info log
	return infoLogString;
}

GLuint Shader::compileShader(std::string shaderSource, GLenum shaderType)
{
	std::string shaderTypeString;
	switch (shaderType)
	{
	case GL_VERTEX_SHADER:
		shaderTypeString = "GL_VERTEX_SHADER";
		break;
	case GL_FRAGMENT_SHADER:
		shaderTypeString = "GL_FRAGMENT_SHADER";
		break;
	case GL_GEOMETRY_SHADER:
		throw std::runtime_error("Geometry shaders are unsupported at this time.");
		break;
	default:
		throw std::runtime_error("Bad shader type enum in compileShader.");
		break;
	}

	// Generate a shader id
	// Note: Shader id will be non-zero if successfully created.
	GLuint shaderId = glCreateShader(shaderType);
	if (shaderId == 0)
	{
		// Display the shader log via a runtime_error
		throw std::runtime_error("Could not create shader of type " + shaderTypeString + ": " + getInfoLog(ObjectType::SHADER, shaderId));
	}

	// Get the source string as a pointer to an array of characters
	const char *shaderSourceChars = shaderSource.c_str();

	// Attach the GLSL source code to the shader
	// Params: GLuint shader, GLsizei count, const GLchar **string, const GLint *length
	// Note: The pointer to an array of source chars will be null terminated, so we don't need to specify the length and can instead use NULL.
	glShaderSource(shaderId, 1, &shaderSourceChars, NULL);

	// Compile the shader
	glCompileShader(shaderId);

	// Check the compilation status and throw a runtime_error if shader compilation failed
	GLint shaderStatus;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shaderStatus);
	if (shaderStatus == GL_FALSE)
	{
		std::cout << shaderTypeString << " compilation failed: " << getInfoLog(ObjectType::SHADER, shaderId) << std::endl;
	}
	else
	{
		if (DEBUG)
		{
			std::cout << shaderTypeString << " shader compilation successful." << std::endl;
		}
	}

	// If everything went well, return the shader id
	return shaderId;
}

/* UTILITIES*/

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(uniform(name), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(uniform(name), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(uniform(name), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(uniform(name), 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(uniform(name), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(uniform(name), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(uniform(name), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(uniform(name), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(uniform(name), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(uniform(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(uniform(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(uniform(name), 1, GL_FALSE, &mat[0][0]);
}
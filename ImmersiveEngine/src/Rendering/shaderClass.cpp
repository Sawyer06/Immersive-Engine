#include"shaderClass.h"

/// Read and return the contents of a text file.
std::string getFileContents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	// Read the contents of the shader data files.
	std::string vertexCode = getFileContents(vertexFile);
	std::string fragmentCode = getFileContents(fragmentFile);
	
	// Convert string to char.
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Create the vertex shader.
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	// Create the fragment shader.
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	// Create the shader program.
	ID = glCreateProgram();

	// Attaches the data to the shader program.
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	compileErrors(ID, "PROGRAM");
	
	// Links the shader program for execution.
	glLinkProgram(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
	glUseProgram(ID); // Installs a program object as part of current rendering state.
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}

/// Set uniform values for the shader.
void Shader::setBool(const std::string& name, bool value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setInt(const std::string& name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, glm::vec2 value)
{
	glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shader::setVec3(const std::string& name, glm::vec3 value)
{
	glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string& name, glm::vec4 value)
{
	glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.a);
}

void Shader::setMat4(const std::string& name, const glm::mat4 value)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

/// Returns the shader uniform variable location from the cache, looking by name. Search for it and add to cache if not already in there.
GLint Shader::getUniformLocation(const std::string& name)
{
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end()) // If already found in the cache, go ahead and return.
    {
        return m_uniformLocationCache[name];
    }
    GLint location = glGetUniformLocation(ID, name.c_str()); // Get the uniform location if not already cached.
    if (location == -1) // If the name passed does not exist as a uniform.
    {
        std::cerr << "SHADER_UNIFORM_ERROR name '" << name << "' not found.";
    }
    m_uniformLocationCache[name] = location; // Create the new uniform item in cache.
    return location;
}

/// Shader error checking.
void Shader::compileErrors(unsigned int shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "SHADER_COMPILATION_ERROR for: " << type << "\n" << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "SHADER_LINKING_ERROR for: " << type << "\n" << std::endl;
		}
	}
}


#include "GLRenderer.hpp"

namespace SVR
{
namespace OpenGL
{

ShaderPtr Shader::loadFromFile(const std::string &path, GLenum type)
{
    // Lod the shader source code.
    std::vector<char> sourceCode;
    if(!loadTextFileInto(path, sourceCode))
        return ShaderPtr();

    // Create and compile the shader.
    auto shader = glCreateShader(type);
    const char *sourceCodePtr = &sourceCode[0];
    glShaderSource(shader, 1, &sourceCodePtr, nullptr);
    glCompileShader(shader);

    // Check for errors
    int res;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
    if(res != GL_TRUE)
    {
        // Get the info log size.
        int infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        // Read the info log
        std::unique_ptr<char[]> infoLog(new char[infoLogLength + 1]);
        glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.get());

        // FIXME: Use a logging function
        fprintf(stderr, "Failed to compile shader(%s):\n%s\n", path.c_str(), infoLog.get());

        // Delete the shader
        glDeleteShader(shader);
        return ShaderPtr();
    }

    return ShaderPtr(new Shader(shader));
}

Shader::Shader(GLuint handle)
    : handle(handle)
{
}

Shader::~Shader()
{
}

GLuint Shader::getHandle()
{
    return handle;
}

void Shader::destroy()
{
    glDeleteShader(handle);
}

Program::Program(GLuint handle, const ShaderPtr &vertexShader, const ShaderPtr &fragmentShader)
    : handle(handle), vertexShader(vertexShader), fragmentShader(fragmentShader)
{
    loadUniformIds();
}

Program::~Program()
{
}

GLuint Program::getHandle()
{
    return handle;
}

void Program::destroy()
{
    glUseProgram(0);
    glDetachShader(handle, vertexShader->getHandle());
    glDetachShader(handle, fragmentShader->getHandle());
    glDeleteProgram(handle);
    vertexShader->destroy();
    fragmentShader->destroy();
}

void Program::bindVertexAttributes(GLuint program)
{
    glBindAttribLocation(program, (GLuint)VertexAttribute::Position, "vPosition");
    glBindAttribLocation(program, (GLuint)VertexAttribute::TexCoord, "vTexCoord");
    glBindAttribLocation(program, (GLuint)VertexAttribute::Color, "vColor");
}

ProgramPtr Program::loadFromFile(const std::string &vertexPath, const std::string &fragmentPath)
{
    // Load the vertex shader.
    auto vertexShader = Shader::loadFromFile(vertexPath, GL_VERTEX_SHADER);
    if(!vertexShader)
        return ProgramPtr();

    // Load the fragment shader.
    auto fragmentShader = Shader::loadFromFile(fragmentPath, GL_FRAGMENT_SHADER);
    if(!fragmentShader)
    {
        vertexShader->destroy();
        return ProgramPtr();
    }

    // Create and link the program
    auto program = glCreateProgram();
    glAttachShader(program, vertexShader->getHandle());
    glAttachShader(program, fragmentShader->getHandle());
    bindVertexAttributes(program);
    glLinkProgram(program);

    // Check for errors
    int res;
    glGetProgramiv(program, GL_LINK_STATUS, &res);
    if(res != GL_TRUE)
    {
        // Get the info log size.
        int infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        // Read the info log
        std::unique_ptr<char[]> infoLog(new char[infoLogLength + 1]);
        glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.get());

        // FIXME: Use a logging function
        fprintf(stderr, "Failed to link program(%s ; %s):\n%s\n", vertexPath.c_str(), fragmentPath.c_str(), infoLog.get());

        // Delete the program and the shaders.
        glDetachShader(program, vertexShader->getHandle());
        glDetachShader(program, fragmentShader->getHandle());
        glDeleteProgram(program);
        vertexShader->destroy();
        fragmentShader->destroy();
        return ProgramPtr();
    }

    return ProgramPtr(new Program(program, vertexShader, fragmentShader));
}

void Program::loadUniformIds()
{
    char buffer[256];
    int numuniforms;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &numuniforms);

    for(int i = 0; i < numuniforms; ++i)
    {
        GLint size;
        GLenum type;
        glGetActiveUniform(handle, i, sizeof(buffer), nullptr, &size, &type, buffer);

        uniformNameMap.insert(std::make_pair(buffer, i));
    }
}

GLint Program::uniformId(const std::string &name)
{
    auto it = uniformNameMap.find(name);
    return (it != uniformNameMap.end()) ? it->second : -1;
}

void Program::uniformFloat(const std::string &name, float value)
{
    auto location = uniformId(name);
    glUniform1f(location, value);
}

void Program::uniformVec2(const std::string &name, const glm::vec2 &vector)
{
    auto location = uniformId(name);
    glUniform2fv(location, 1, (float*)&vector);
}

void Program::uniformVec3(const std::string &name, const glm::vec3 &vector)
{
    auto location = uniformId(name);
    glUniform3fv(location, 1, (float*)&vector);
}

void Program::uniformVec4(const std::string &name, const glm::vec3 &vector)
{
    auto location = uniformId(name);
    glUniform4fv(location, 1, (float*)&vector);
}

void Program::uniformMat4(const std::string &name, const glm::mat4 &matrix)
{
    auto location = uniformId(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

} // namespace OpenGL
} // namespace SVR


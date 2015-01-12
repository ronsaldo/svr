#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SVR/LoadUtilities.hpp"
#include "SVR/Logging.hpp"
#include "SVR/Renderer.hpp"
#include "SVR/Mesh.hpp"
#include "SVR/VertexAttributes.hpp"

namespace SVR
{
namespace OpenGL
{

DECLARE_CLASS(Shader);
DECLARE_CLASS(Program);

/**
 * Shader
 */
class Shader
{
public:
    ~Shader();

    static ShaderPtr loadFromFile(const std::string &path, GLenum type);

    GLuint getHandle();
    void destroy();

private:
    Shader(GLuint handle);

    GLuint handle;
};

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

/**
 * Program
 */
class Program
{
public:
    ~Program();

    static ProgramPtr loadFromFile(const std::string &vertexPath, const std::string &fragmentPath);

    GLuint getHandle();
    void destroy();

private:
    static void bindVertexAttributes(GLuint program);
    Program(GLuint handle, const ShaderPtr &vertexShader, const ShaderPtr &fragmentShader);

    GLuint handle;
    ShaderPtr vertexShader;
    ShaderPtr fragmentShader;
};

Program::Program(GLuint handle, const ShaderPtr &vertexShader, const ShaderPtr &fragmentShader)
    : handle(handle), vertexShader(vertexShader), fragmentShader(fragmentShader)
{
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
    glBindAttribLocation(program, (GLuint)VertexAttributes::Position, "vPosition");
    glBindAttribLocation(program, (GLuint)VertexAttributes::TexCoord, "vTexCoord");
    glBindAttribLocation(program, (GLuint)VertexAttributes::Color, "vColor");
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

/**
 * The OpenGL renderer
 */
class GLRenderer: public Renderer
{
public:
    GLRenderer();
    ~GLRenderer();

    virtual bool initialize(int argc, const char **argv);
    virtual void shutdown();

    virtual void setScreenSize(const glm::vec2 &newScreenSize);

    virtual void clearColor(const glm::vec4 &color);
    virtual void clear();

    virtual void setColor(const glm::vec4 &color);
    virtual void drawLine(const glm::vec2 &start, const glm::vec2 &end);
    virtual void drawTriangle(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c);

    virtual void flushCommands();

private:
    bool loadPrograms();

    ProgramPtr colorProgram;
    ProgramPtr texturedProgram;

    Mesh2D mesh;

    // Render state
    glm::vec2 screenSize;
    glm::mat4 projectionMatrix;
    glm::vec4 currentColor;
};

GLRenderer::GLRenderer()
{
}

GLRenderer::~GLRenderer()
{
}

bool GLRenderer::initialize(int argc, const char **argv)
{
    auto glewError = glewInit();
    if (GLEW_OK != glewError)
    {
        logError("Failed to load OpenGL extensions");
        return false;
    }

    if(!loadPrograms())
        return false;

    return true;
}

void GLRenderer::shutdown()
{
}

bool GLRenderer::loadPrograms()
{
    colorProgram = Program::loadFromFile("data/shaders/color.glslv", "data/shaders/color.glslf");
    texturedProgram = Program::loadFromFile("data/shaders/textured.glslv", "data/shaders/textured.glslf");

    return colorProgram && texturedProgram;
}

void GLRenderer::setScreenSize(const glm::vec2 &newScreenSize)
{
    screenSize = newScreenSize;
    projectionMatrix = glm::ortho(0.0f, screenSize.x, 0.0f, screenSize.y, -1.0f, 1.0f);
}

void GLRenderer::clearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void GLRenderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderer::setColor(const glm::vec4 &color)
{
    currentColor = color;
}

void GLRenderer::drawLine(const glm::vec2 &start, const glm::vec2 &end)
{
    // TODO:
}

void GLRenderer::drawTriangle(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c)
{
    // TODO: 
}

void GLRenderer::flushCommands()
{
    // TODO:
}

} // namespace OpenGL

RendererPtr createRenderer()
{
    return std::make_shared<OpenGL::GLRenderer> ();
}
} // namespace SVR


#ifndef _SVR_GL_RENDERER_HPP_
#define _SVR_GL_RENDERER_HPP_

#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <stdlib.h>

#include "SVR/LoadUtilities.hpp"
#include "SVR/Logging.hpp"
#include "SVR/Renderer.hpp"
#include "SVR/Mesh.hpp"
#include "SVR/MeshBuilder.hpp"
#include "SVR/VertexAttribute.hpp"
#include "SVR/Texture.hpp"

namespace SVR
{
namespace OpenGL
{
DECLARE_CLASS(Shader);
DECLARE_CLASS(Program);
DECLARE_CLASS(BufferObject);
DECLARE_CLASS(GLTexture2D);

/**
 * Maps a primitive mode constant into the opengl contant
 */
inline GLenum mapPrimitiveMode(PrimitiveMode primitiveMode)
{
    switch(primitiveMode)
    {
    case PrimitiveMode::Points: return GL_POINTS;
    case PrimitiveMode::Lines: return GL_LINES;
    case PrimitiveMode::LineStrip: return GL_LINE_STRIP;
    case PrimitiveMode::Triangles: return GL_TRIANGLES;
    case PrimitiveMode::TriangleFan: return GL_TRIANGLE_FAN;
    case PrimitiveMode::TriangleStrip: return GL_TRIANGLE_STRIP;
    default:
        abort();
    }
}

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

/**
 * Program
 */
class Program
{
public:
    ~Program();

    static ProgramPtr loadFromFile(const std::string &vertexPath, const std::string &fragmentPath);

    void uniformMat4(const std::string &name, const glm::mat4 &matrix);

    GLuint getHandle();
    void destroy();

private:
    
    static void bindVertexAttributes(GLuint program);
    Program(GLuint handle, const ShaderPtr &vertexShader, const ShaderPtr &fragmentShader);

    void loadUniformIds();
    GLint uniformId(const std::string &name);

    GLuint handle;
    ShaderPtr vertexShader;
    ShaderPtr fragmentShader;

    typedef std::map<std::string, GLint> VariableNameMap;
    VariableNameMap uniformNameMap;
};

/**
 * Buffer object 
 */
class BufferObject
{
public:
    ~BufferObject();
    static BufferObjectPtr create(GLenum target, GLenum usage);

    GLuint getHandle() const;

    void destroy();

    void bind();

    void uploadData(size_t size, const void *data);

private:
    BufferObject(GLenum target, GLenum usage, GLuint handle);

    GLenum target;
    GLenum usage;
    GLuint handle;
};

/**
 * OpenGL texture 2D
 */
class GLTexture2D: public Texture2D
{
public:
    ~GLTexture2D();

    virtual TextureFilter getMagnificationFilter() const;
    virtual TextureFilter getMinificationFilter() const;
    virtual void setMagnificationFilter(TextureFilter newFilter);
    virtual void setMinificationFilter(TextureFilter newFilter);

    virtual PixelFormat getPixelFormat() const;

    virtual void *getHandle() const;

    virtual int getWidth() const;
    virtual int getHeight() const;

    virtual void allocateInDevice();
    virtual void upload(PixelFormat format, size_t size, const void *data);

    void bind();

    static GLTexture2DPtr create(int width, int height, PixelFormat pixelFormat);

private:
    GLTexture2D(GLuint handle, int width, int height, PixelFormat pixelFormat);

    void update();

    GLuint handle;
    int width, height;
    PixelFormat pixelFormat;

    TextureFilter magnificationFilter;
    TextureFilter minificationFilter;

    bool needsUpdate;
};

} // namespace OpenGL
} // namespace SVR

#endif //_SVR_GL_RENDERER_HPP_

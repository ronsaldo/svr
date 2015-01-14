#include "GLRenderer.hpp"

namespace SVR
{
namespace OpenGL
{

inline GLenum mapTextureFilter(TextureFilter filter)
{
    switch(filter)
    {
    case TextureFilter::Nearest:
        return GL_NEAREST;
    case TextureFilter::Linear:
        return GL_LINEAR;
    case TextureFilter::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case TextureFilter::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    case TextureFilter::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case TextureFilter::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    default:
        abort();
    }
}

inline GLenum mapInternalFormat(PixelFormat pixelFormat)
{
    switch(pixelFormat)
    {
    case PixelFormat::L8:
        return GL_LUMINANCE8;
    case PixelFormat::I8:
        return GL_INTENSITY8;
    case PixelFormat::R8:
        return GL_R8;
    case PixelFormat::RG8:
        return GL_RG8;
    case PixelFormat::RGB8:
        return GL_RGB8;
    case PixelFormat::RGBA8:
        return GL_RGBA8;

    case PixelFormat::R32F:
        return GL_R32F;
    case PixelFormat::RG32F:
        return GL_RG32F;
    case PixelFormat::RGBA32F:
        return GL_RGBA32F;
    default:
        abort();
    }
}

inline GLenum mapFormat(PixelFormat pixelFormat)
{
    switch(pixelFormat)
    {
    case PixelFormat::L8:
        return GL_LUMINANCE;
    case PixelFormat::I8:
        return GL_INTENSITY;
    case PixelFormat::R8:
        return GL_R;
    case PixelFormat::RG8:
        return GL_RG;
    case PixelFormat::RGB8:
        return GL_RGB;
    case PixelFormat::RGBA8:
        return GL_RGBA;

    case PixelFormat::R32F:
        return GL_R;
    case PixelFormat::RG32F:
        return GL_RG;
    case PixelFormat::RGBA32F:
        return GL_RGBA;
    default:
        abort();
    }
}

inline GLenum mapType(PixelFormat pixelFormat)
{
    switch(pixelFormat)
    {
    case PixelFormat::L8:
    case PixelFormat::I8:
    case PixelFormat::R8:
    case PixelFormat::RG8:
    case PixelFormat::RGB8:
    case PixelFormat::RGBA8:
        return GL_UNSIGNED_BYTE;

    case PixelFormat::R32F:
    case PixelFormat::RG32F:
    case PixelFormat::RGBA32F:
        return GL_FLOAT;
    default:
        abort();
    }
}

GLTexture2D::GLTexture2D(GLuint handle, int width, int height, PixelFormat pixelFormat)
    : handle(handle), width(width), height(height), pixelFormat(pixelFormat)
{
    magnificationFilter = TextureFilter::Linear;
    minificationFilter = TextureFilter::Linear;
    update();
}

GLTexture2D::~GLTexture2D()
{
}

TextureFilter GLTexture2D::getMagnificationFilter() const
{
    return magnificationFilter;
}

TextureFilter GLTexture2D::getMinificationFilter() const
{
    return minificationFilter;
}

void GLTexture2D::setMagnificationFilter(TextureFilter newFilter)
{
    magnificationFilter = newFilter;
    needsUpdate = true;
}

void GLTexture2D::setMinificationFilter(TextureFilter newFilter)
{
    minificationFilter = newFilter;
    needsUpdate = true;
}

PixelFormat GLTexture2D::getPixelFormat() const
{
    return pixelFormat;
}

void *GLTexture2D::getHandle() const
{
    return reinterpret_cast<void*> (size_t(handle));
}

int GLTexture2D::getWidth() const
{
    return width;
}

int GLTexture2D::getHeight() const
{
    return height;
}

void GLTexture2D::bind()
{
    glBindTexture(GL_TEXTURE_2D, handle);
}

void GLTexture2D::allocateInDevice()
{
    update();

    auto internalFormat = mapInternalFormat(pixelFormat);
    auto format = mapFormat(pixelFormat);
    auto type = mapType(pixelFormat);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
}

void GLTexture2D::upload(PixelFormat imageFormat, size_t size, const void *data)
{
    update();

    auto internalFormat = mapInternalFormat(pixelFormat);
    auto format = mapFormat(imageFormat);
    auto type = mapType(imageFormat);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);

}

void GLTexture2D::update()
{
    bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mapTextureFilter(minificationFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mapTextureFilter(magnificationFilter));
    needsUpdate = false;
}

GLTexture2DPtr GLTexture2D::create(int width, int height, PixelFormat pixelFormat)
{
    GLuint handle;
    glGenTextures(1, &handle);

    return GLTexture2DPtr(new GLTexture2D(handle, width, height, pixelFormat));
}

} // namespace OpenGL
} // namespace SVR
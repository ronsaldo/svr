#include "GLRenderer.hpp"

namespace SVR
{
namespace OpenGL
{

BufferObject::BufferObject(GLenum target, GLenum usage, GLuint handle)
    : target(target), usage(usage), handle(handle)
{
}

BufferObject::~BufferObject()
{
}

BufferObjectPtr BufferObject::create(GLenum target, GLenum usage)
{
    GLuint handle;
    glGenBuffers(1, &handle);
    return BufferObjectPtr(new BufferObject(target, usage, handle));
}

GLuint BufferObject::getHandle() const
{
    return handle;
}

void BufferObject::destroy()
{
    glDeleteBuffers(1, &handle);
}

void BufferObject::bind()
{
    glBindBuffer(target, handle);
}

void BufferObject::uploadData(size_t size, const void *data)
{
    bind();
    glBufferData(target, size, data, usage);
}

} // namespace OpenGL
} // namespace SVR

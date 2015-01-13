#ifndef _SVR_TEXTURE_HPP_
#define _SVR_TEXTURE_HPP_

#include "SVR/Interface.hpp"
#include "SVR/TextureFilter.hpp"
#include "SVR/PixelFormat.hpp"

namespace SVR
{
DECLARE_INTERFACE(Texture);
DECLARE_INTERFACE(Texture1D);
DECLARE_INTERFACE(Texture2D);

/**
 * Texture
 */
struct Texture: public Interface
{
    virtual TextureFilter getMagnificationFilter() const = 0;
    virtual TextureFilter getMinificationFilter() const = 0;
    virtual void setMagnificationFilter(TextureFilter newFilter) = 0;
    virtual void setMinificationFilter(TextureFilter newFilter) = 0;

    virtual PixelFormat getPixelFormat() const = 0;

    virtual void *getHandle() const = 0;

    virtual void allocateInDevice() = 0;
    virtual void upload(PixelFormat format, size_t size, const void *data) = 0;
};

/**
 * Texture 1D
 */
struct Texture1D: public Texture
{
    virtual int getWidth() const = 0;
};

/**
 * Texture 2D
 */
struct Texture2D: public Texture
{
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};
} // namespace SVR

#endif //_SVR_TEXTURE_HPP_

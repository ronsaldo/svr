#ifndef _SVR_FRAMEBUFFER_HPP_
#define _SVR_FRAMEBUFFER_HPP_

#include "SVR/Texture.hpp"

namespace SVR
{
DECLARE_INTERFACE(Framebuffer);

/**
 * Framebuffer attachment
 */
enum class FramebufferAttachment
{
    Color=0,
    Depth,
    Stencil
};

/**
 * Framebuffer
 */
struct Framebuffer: Interface
{
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    virtual void attachTexture(FramebufferAttachment attachmentPoint, const Texture2DPtr &texture) = 0;
    virtual void activate() = 0;

};

} // namespace SVR

#endif //_SVR_FRAMEBUFFER_HPP_


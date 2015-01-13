#ifndef _SVR_PIXEL_FORMAT_HPP_
#define _SVR_PIXEL_FORMAT_HPP_

namespace SVR
{

/**
 * Texture and image pixel format
 */
enum class PixelFormat
{
    L8 = 0,
    I8,
    R8,
    RG8,
    RGB8,
    RGBA8,

    R32F,
    RG32F,
    RGBA32F,
};

} // namespace SVR
#endif //_SVR_PIXEL_FORMAT_HPP_

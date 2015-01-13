#ifndef _TEXTURE_FILTER_HPP_
#define _TEXTURE_FILTER_HPP_

namespace SVR
{
/**
 * Texture filter enum
 */
enum class TextureFilter
{
    Nearest = 0,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
};

} // namespace SVR

#endif //_TEXTURE_FILTER_HPP_


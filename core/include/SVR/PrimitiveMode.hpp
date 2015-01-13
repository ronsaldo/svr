#ifndef _SVR_PRIMITIVE_MODE_HPP_
#define _SVR_PRIMITIVE_MODE_HPP_

namespace SVR
{

/**
 * Primitive mode
 */
enum class PrimitiveMode
{
    Points=0,
    Lines,
    LineStrip,
    Triangles,
    TriangleFan,
    TriangleStrip,
};

} // namespace SVR

#endif //_SVR_PRIMITIVE_MODE_HPP_


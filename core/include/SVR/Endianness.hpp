#ifndef _SVR_ENDIANNESS_HPP_
#define _SVR_ENDIANNESS_HPP_

#include <stdint.h>

namespace SVR
{
namespace detail
{

template<typename T>
struct SwapBytes;

template<>
struct SwapBytes<uint8_t>
{
    static uint8_t apply(const uint8_t &v)
    {
        return v;
    }
};

template<>
struct SwapBytes<uint16_t>
{
    static uint16_t apply(const uint16_t &v)
    {
        return (v >> 8) | ((v & 0xFF) << 8);
    }
};

template<>
struct SwapBytes<uint32_t>
{
    static uint32_t apply(const uint32_t &v)
    {
        return ((v >> 24) & 0x000000FF) |
               ((v >>  8) & 0x0000FF00) |
               ((v <<  8) & 0x00FF0000) |
               ((v << 24) & 0xFF000000);
    }
};

template<>
struct SwapBytes<uint64_t>
{
    static uint64_t apply(const uint64_t &v)
    {
        return ((v >> 56) & 0x00000000000000FFul) |
               ((v >> 40) & 0x000000000000FF00ul) |
               ((v >> 24) & 0x0000000000FF0000ul) |
               ((v >>  8) & 0x00000000FF000000ul) |
               ((v <<  8) & 0x000000FF00000000ul) |
               ((v << 24) & 0x0000FF0000000000ul) |
               ((v << 40) & 0x00FF000000000000ul) |
               ((v << 56) & 0xFF00000000000000ul);
    }
};

template<>
struct SwapBytes<float>
{
    static float cast(const uint32_t &v)
    {
        return reinterpret_cast<const float &> (v);
    }

    static float apply(const float &v)
    {
        return cast(SwapBytes<uint32_t>::apply(reinterpret_cast<const uint32_t &> (v))); 
    }
};

template<>
struct SwapBytes<double>
{
    static double cast(const uint64_t &v)
    {
        return reinterpret_cast<const double &> (v);
    }

    static double apply(const double &v)
    {
        return cast(SwapBytes<uint64_t>::apply(reinterpret_cast<const uint64_t &> (v)));
    }
};

template<>
struct SwapBytes<int16_t> : SwapBytes<uint16_t> {};

template<>
struct SwapBytes<int32_t> : SwapBytes<uint32_t> {};

template<>
struct SwapBytes<int64_t> : SwapBytes<uint64_t> {};


} // namespace detail

template<typename T>
T swapBytes(const T &source)
{
    return detail::SwapBytes<T>::apply(source);
}

} // SVR

#endif //_SVR_ENDIANNESS_HPP_


#ifndef _CHANNEL_TYPES_HPP_
#define _CHANNEL_TYPES_HPP_

#include <stdint.h>
namespace SVR
{
namespace detail
{

template<typename T>
struct NormalizationConstant;

template<>
struct NormalizationConstant<uint8_t>
{
    static double apply()
    {
        return 255.0;
    }
};

template<>
struct NormalizationConstant<int16_t>
{
    static double apply()
    {
        return 1<<15;
    }
};

template<>
struct NormalizationConstant<int32_t>
{
    static double apply()
    {
        return 1<<31;
    }
};

template<>
struct NormalizationConstant<int64_t>
{
    static double apply()
    {
        return int64_t(1) << int64_t(63);
    }
};

template<>
struct NormalizationConstant<float>
{
    static double apply()
    {
        return 1.0;
    }
};

template<>
struct NormalizationConstant<double>
{
    static double apply()
    {
        return 1.0;
    }
};

} // namespace detail

template<typename T>
double normalizationConstant()
{
    return detail::NormalizationConstant<T>::apply();
}

} // namespace SVR

#endif //_SVR_CHANNEL_TYPES_HPP_


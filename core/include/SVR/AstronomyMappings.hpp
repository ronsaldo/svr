#ifndef _SVR_ASTRONOMY_MAPPINGS_HPP_
#define _SVR_ASTRONOMY_MAPPINGS_HPP_

#include <algorithm>
#include <math.h>
#include "SVR/FitsFile.hpp"
#include "SVR/Endianness.hpp"
#include "SVR/ChannelTypes.hpp"

namespace SVR
{

/**
 * Simple mapping base class
 */
struct SimpleMapping
{
    double minValue, maxValue, invMaxValue;
};

/**
 * Linear mapping
 */
struct LinearMapping : SimpleMapping
{
    void setup(double minValue, double maxValue)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->invMaxValue = 1.0 / maxValue;
    }

    double map(double v)
    {
        return v*invMaxValue;
    }

    double unmap(double v)
    {
        return v*maxValue;
    }
};

/**
 * Logarithmic mapping.
 */
struct LogMapping : SimpleMapping
{
    double numberOfColors, exponent, norm;

    void setup(double minValue, double maxValue)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->invMaxValue = 1.0 / maxValue;

	    this->exponent = log(maxValue - minValue);
	    this->numberOfColors = 256.0/4.0;

        if(exponent >= 0.0)
            this->norm = 1.0 / log(this->exponent*this->numberOfColors + 1.0);
        else
            this->norm = 1.0 / log(1.0 - this->exponent*this->numberOfColors);

    }

    double map(double v)
    {
        if(exponent >= 0)
		    return this->norm*log(this->exponent*this->numberOfColors*v*this->invMaxValue + 1.0);
        else
		    return this->norm*log(1.0 - this->exponent*this->numberOfColors*v*this->invMaxValue);
    }

    double unmap(double v)
    {
        if(exponent >= 0)
            return this->maxValue * (exp(v / this->norm) - 1.0) / (this->exponent * this->numberOfColors);
        else
            return this->maxValue * (1.0 - exp(v / this->norm)) / (this->exponent * this->numberOfColors);
    }
};

/**
 * Square mapping.
 */
struct SquareMapping : SimpleMapping
{
    double norm;

    void setup(double minValue, double maxValue)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->invMaxValue = 1.0 / maxValue;
        this->norm = 1.0;
    }

    double map(double v)
    {
        auto nv = v*invMaxValue;
        return this->norm*nv*nv;
    }

    double unmap(double v)
    {
        auto nv = v/this->norm;
        return sqrt(nv)*maxValue;
    }
};

/**
 * Square root mapping.
 */
struct SquareRootMapping : SimpleMapping
{
    double norm;

    void setup(double minValue, double maxValue)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->invMaxValue = 1.0 / maxValue;
        this->norm = 1.0;
    }

    double map(double v)
    {
        auto nv = v*invMaxValue;
        return this->norm*sqrt(nv);
    }

    double unmap(double v)
    {
        auto nv = v/this->norm;
        return nv*nv*maxValue;
    }
};

/**
 * Hiperbolic sine
 */
inline double sinh(double x)
{
    return 0.5*(exp(x) - exp(-x));
}

/**
 * Hiperbolic arc sine.
 */
inline double arcsinh(double x)
{
    return log(x + sqrt(x*x + 1.0));
}


/**
 * Sinh mapping.
 */
struct SinhMapping : SimpleMapping
{
    double norm;

    void setup(double minValue, double maxValue)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->invMaxValue = 1.0 / maxValue;
        this->norm = 1.0;
    }

    double map(double v)
    {
        return this->norm*sinh(v*this->invMaxValue);
    }

    double unmap(double v)
    {
        return this->maxValue*arcsinh(v/this->norm);
    }
};

/**
 * ASinh mapping.
 */
struct ASinhMapping : SimpleMapping
{
    double norm;

    void setup(double minValue, double maxValue)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        this->invMaxValue = 1.0 / maxValue;
        this->norm = 1.0;
    }

    double map(double v)
    {
        return this->norm*arcsinh(v*this->invMaxValue);
    }

    double unmap(double v)
    {
        return this->maxValue*sinh(v/this->norm);
    }
};

namespace detail
{
template<typename T>
T minIgnoreNaN(T a, T b)
{
	if(isnan(a))
		return b;
	else if(isnan(b))
		return a;
	else if(a < b)
		return a;
	else
		return b;
}

template<typename T>
T maxIgnoreNaN(T a, T b)
{
	if(isnan(a))
		return b;
	else if(isnan(b))
		return a;
	else if(a > b)
		return a;
	else
		return b;
}

template<typename FromType, typename Mapping, typename ToType>
void mapFromTypeInto(Mapping &mapping, FitsFile *input, ToType *dest, SliceRange x, SliceRange y, SliceRange z)
{
    int minX = x.start;
    int maxX = minX + x.size;
    int minY = y.start;
    int maxY = minY + y.size;
    int minZ = z.start;
    int maxZ = minZ + z.size;

    int sourcePitch = input->getWidth();
    int sourceSlicePitch = sourcePitch*input->getHeight();

    auto sourceStart = reinterpret_cast<const FromType*> (input->getImageData()) +
                minZ*sourceSlicePitch + minY*sourcePitch + minX;
    auto normConstant = normalizationConstant<ToType> ();

    // Compute min and max
    FromType minValue, maxValue;
    minValue = maxValue = (swapBytes<FromType> (*sourceStart));
    auto sourceSlice = sourceStart;
    for(int z = minZ; z < maxZ; ++z, sourceSlice += sourceSlicePitch)
    {
        auto sourceRow = sourceSlice;
        for(int y = minY; y < maxY; ++y, sourceRow += sourcePitch)
        {
            auto sourceElement = sourceRow;
            for(int x = minX; x < maxX; ++x, ++sourceElement)
            {
                auto value = swapBytes<FromType> (*sourceElement);
                minValue = minIgnoreNaN(minValue, value);
                maxValue = maxIgnoreNaN(maxValue, value);
            }
        }
    }

    mapping.setup(minValue, maxValue);

    sourceSlice = sourceStart;
    for(int z = minZ; z < maxZ; ++z, sourceSlice += sourceSlicePitch)
    {
        auto sourceRow = sourceSlice;
        for(int y = minY; y < maxY; ++y, sourceRow += sourcePitch)
        {
            auto sourceElement = sourceRow;
            for(int x = minX; x < maxX; ++x, ++sourceElement)
            {
                *dest++ = ToType(mapping.map(swapBytes<FromType> (*sourceElement)) * normConstant);
            }
        }
    }
}

template<typename FromType, typename ToType, typename Mapping>
void mapFromTypeToTypeInto(Mapping &mapping, FitsFile *input, FitsFile *output, SliceRange x, SliceRange y, SliceRange z)
{
    auto numberOfElements = input->getNumberOfElements();
    auto src = reinterpret_cast<const FromType*> (input->getImageData());
    auto dest = reinterpret_cast<ToType*> (output->getImageData());
    auto normConstant = normalizationConstant<ToType> ();
    assert(input->getNumberOfElements() == output->getNumberOfElements());

    // Compute min and max
    FromType minValue, maxValue;
    minValue = maxValue = (swapBytes<FromType> (*src++));
    for(size_t i = 1; i < numberOfElements; ++i)
    {
        auto value = swapBytes<FromType> (*src++);
        minValue = std::min(minValue, value);
        maxValue = std::max(maxValue, value);
    }

    mapping.setup(minValue, maxValue);

    src = reinterpret_cast<const FromType*> (input->getImageData());
    for(size_t i = 0; i < numberOfElements; ++i)
        *dest++ = swapBytes<ToType> (ToType(mapping.map(swapBytes<FromType> (*src++)) * normConstant));
}

template<typename FromType, typename Mapping>
void mapFromTypeInto(Mapping &mapping, FitsFile *input, FitsFile *output, SliceRange x, SliceRange y, SliceRange z)
{
    switch(output->getFormat())
    {
    case FitsFormat::UInt8:
        mapFromTypeToTypeInto<FromType, unsigned char> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Int16:
        mapFromTypeToTypeInto<FromType, int16_t> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Int32:
        mapFromTypeToTypeInto<FromType, int32_t> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Int64:
        mapFromTypeToTypeInto<FromType, int64_t> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Float:
        mapFromTypeToTypeInto<FromType, float> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Double:
        mapFromTypeToTypeInto<FromType, double> (mapping, input, output, x, y, z);
        break;
    }
}



} // namespace detail

template<typename Mapping>
void mapFitsIntoFits(Mapping &mapping, FitsFile *input, FitsFile *output, SliceRange x, SliceRange y, SliceRange z)
{
    switch(input->getFormat())
    {
    case FitsFormat::UInt8:
        detail::mapFromTypeInto<unsigned char> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Int16:
        detail::mapFromTypeInto<int16_t> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Int32:
        detail::mapFromTypeInto<int32_t> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Int64:
        detail::mapFromTypeInto<int64_t> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Float:
        detail::mapFromTypeInto<float> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Double:
        detail::mapFromTypeInto<double> (mapping, input, output, x, y, z);
        break;
    }
}

template<typename Mapping, typename Output>
void mapFitsInto(Mapping &mapping, FitsFile *input, Output output, SliceRange x, SliceRange y, SliceRange z)
{
    switch(input->getFormat())
    {
    case FitsFormat::UInt8:
        detail::mapFromTypeInto<unsigned char> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Int16:
        detail::mapFromTypeInto<int16_t> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Int32:
        detail::mapFromTypeInto<int32_t> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Int64:
        detail::mapFromTypeInto<int64_t> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Float:
        detail::mapFromTypeInto<float> (mapping, input, output, x, y, z);
        break;
    case FitsFormat::Double:
        detail::mapFromTypeInto<double> (mapping, input, output, x, y, z);
        break;
    }
}
} // namespace SVR

#endif //_SVR_ASTRONOMY_MAPPINGS_HPP_

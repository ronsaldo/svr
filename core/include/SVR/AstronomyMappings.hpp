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
};

namespace detail
{

template<typename FromType, typename Mapping, typename ToType>
void mapFromTypeInto(Mapping mapping, FitsFile *input, ToType *dest)
{
    auto numberOfElements = input->getNumberOfElements();
    auto src = reinterpret_cast<const FromType*> (input->getImageData());
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
        *dest++ = ToType(mapping.map(swapBytes<FromType> (*src++)) * normConstant);
}

template<typename FromType, typename ToType, typename Mapping>
void mapFromTypeToTypeInto(Mapping mapping, FitsFile *input, FitsFile *output)
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
void mapFromTypeInto(Mapping mapping, FitsFile *input, FitsFile *output)
{
    switch(output->getFormat())
    {
    case FitsFormat::UInt8:
        mapFromTypeToTypeInto<FromType, unsigned char> (mapping, input, output);
        break;
    case FitsFormat::Int16:
        mapFromTypeToTypeInto<FromType, int16_t> (mapping, input,output);
        break;
    case FitsFormat::Int32:
        mapFromTypeToTypeInto<FromType, int32_t> (mapping, input, output);
        break;
    case FitsFormat::Int64:
        mapFromTypeToTypeInto<FromType, int64_t> (mapping, input, output);
        break;
    case FitsFormat::Float:
        mapFromTypeToTypeInto<FromType, float> (mapping, input, output);
        break;
    case FitsFormat::Double:
        mapFromTypeToTypeInto<FromType, double> (mapping, input, output);
        break;
    }
}



} // namespace detail

template<typename Mapping>
void mapFitsIntoFits(Mapping mapping, FitsFile *input, FitsFile *output)
{
    switch(input->getFormat())
    {
    case FitsFormat::UInt8:
        detail::mapFromTypeInto<unsigned char> (mapping, input, output);
        break;
    case FitsFormat::Int16:
        detail::mapFromTypeInto<int16_t> (mapping, input, output);
        break;
    case FitsFormat::Int32:
        detail::mapFromTypeInto<int32_t> (mapping, input, output);
        break;
    case FitsFormat::Int64:
        detail::mapFromTypeInto<int64_t> (mapping, input, output);
        break;
    case FitsFormat::Float:
        detail::mapFromTypeInto<float> (mapping, input, output);
        break;
    case FitsFormat::Double:
        detail::mapFromTypeInto<double> (mapping, input, output);
        break;
    }
}

template<typename Mapping, typename Output>
void mapFitsInto(Mapping mapping, FitsFile *input, Output output)
{
    switch(input->getFormat())
    {
    case FitsFormat::UInt8:
        detail::mapFromTypeInto<unsigned char> (mapping, input, output);
        break;
    case FitsFormat::Int16:
        detail::mapFromTypeInto<int16_t> (mapping, input, output);
        break;
    case FitsFormat::Int32:
        detail::mapFromTypeInto<int32_t> (mapping, input, output);
        break;
    case FitsFormat::Int64:
        detail::mapFromTypeInto<int64_t> (mapping, input, output);
        break;
    case FitsFormat::Float:
        detail::mapFromTypeInto<float> (mapping, input, output);
        break;
    case FitsFormat::Double:
        detail::mapFromTypeInto<double> (mapping, input, output);
        break;
    }
}
} // namespace ImageMapping

#endif //_SVR_ASTRONOMY_MAPPINGS_HPP_

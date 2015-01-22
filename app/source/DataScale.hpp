#ifndef _SVR_DATASCALE_HPP_
#define _SVR_DATASCALE_HPP_

#include "SVR/AstronomyMappings.hpp"
#include "SVR/Interface.hpp"

namespace SVR
{
DECLARE_INTERFACE(DataScale)

/**
 * Data scale
 */
struct DataScale: public Interface
{
    virtual void mapFitsIntoFits(FitsFile *input,  FitsFile *output) = 0;

    virtual void mapFitsIntoU8(FitsFile *input, uint8_t *output) = 0;

    virtual double mapValue(double value) = 0;
    virtual double unmapValue(double value) = 0;

};

/**
 * Data scale used in astronomy
 */
template<typename AstronomyMapping>
class AstronomyDataScale: public DataScale
{
public:
    virtual void mapFitsIntoFits(FitsFile *input,  FitsFile *output)
    {
        ::SVR::mapFitsIntoFits(mapping, input, output);
    }

    virtual void mapFitsIntoU8(FitsFile *input, uint8_t *output)
    {
        ::SVR::mapFitsInto(mapping, input, output);
    }

    virtual double mapValue(double value)
    {
        return mapping.map(value);
    }

    virtual double unmapValue(double value)
    {
        return mapping.unmap(value);
    }

private:
    AstronomyMapping mapping;
};

typedef AstronomyDataScale<LinearMapping> LinearDataScale;
typedef AstronomyDataScale<LogMapping> LogDataScale;



} // namespace SVR

#endif //_SVR_DATASCALE_HPP_


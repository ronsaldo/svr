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
    virtual void mapFitsIntoFits(FitsFile *input,  FitsFile *output, SliceRange x=SliceRange(), SliceRange y=SliceRange(), SliceRange z=SliceRange()) = 0;

    virtual void mapFitsIntoU8(FitsFile *input, uint8_t *output, SliceRange x=SliceRange(), SliceRange y=SliceRange(), SliceRange z=SliceRange()) = 0;

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
    virtual void mapFitsIntoFits(FitsFile *input,  FitsFile *output, SliceRange x=SliceRange(), SliceRange y=SliceRange(), SliceRange z=SliceRange())
    {
        ::SVR::mapFitsIntoFits(mapping, input, output, x, y, z);
    }

    virtual void mapFitsIntoU8(FitsFile *input, uint8_t *output, SliceRange x=SliceRange(), SliceRange y=SliceRange(), SliceRange z=SliceRange())
    {
        ::SVR::mapFitsInto(mapping, input, output, x, y, z);
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
typedef AstronomyDataScale<SquareMapping> SquareDataScale;
typedef AstronomyDataScale<SquareRootMapping> SquareRootDataScale;
typedef AstronomyDataScale<SinhMapping> SinhDataScale;
typedef AstronomyDataScale<ASinhMapping> ASinhDataScale;



} // namespace SVR

#endif //_SVR_DATASCALE_HPP_

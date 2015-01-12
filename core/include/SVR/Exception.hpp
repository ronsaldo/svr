#ifndef _SVR_EXCEPTION_HPP
#define _SVR_EXCEPTION_HPP

#include <stdexcept>

namespace SVR
{

/**
 * SVR exception base class.
 */
class Exception: public std::runtime_error
{
public:
    Exception(const std::string &what)
        : std::runtime_error(what)
    {}
};

}

#endif //_SVR_EXCEPTION_HPP

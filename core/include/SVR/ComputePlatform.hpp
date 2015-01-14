#ifndef _SVR_COMPUTE_PLATFORM_HPP_
#define _SVR_COMPUTE_PLATFORM_HPP_

#include "SVR/ComputeProgram.hpp"

namespace SVR
{
DECLARE_INTERFACE(ComputePlatform);

/**
 * Heterogenous compute platform
 */
struct ComputePlatform
{
    virtual bool initialize(int argc, const char **argv) = 0;
    virtual void shutdown() = 0;

    virtual ComputeProgramPtr loadComputeProgramFromFile(const std::string &path) = 0;
};

ComputePlatformPtr createComputePlatform();

} // namespace SVR

#endif //_SVR_COMPUTE_PLATFORM_HPP_

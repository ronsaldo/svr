#ifndef _SVR_COMPUTE_PROGRAM_HPP_
#define _SVR_COMPUTE_PROGRAM_HPP_

#include <string>
#include "SVR/ComputeKernel.hpp"

namespace SVR
{
DECLARE_INTERFACE(ComputeProgram);

/**
 * Compute program
 */
struct ComputeProgram: Interface
{
    virtual bool build(const std::string &options="") = 0;
    virtual void destroy() = 0;

    virtual ComputeKernelPtr createKernel(const std::string &name) = 0;
};

} // namespace SVVR

#endif // _SVR_COMPUTE_PROGRAM_HPP_

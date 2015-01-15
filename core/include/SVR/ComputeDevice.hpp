#ifndef _SVR_COMPUTE_DEVICE_HPP_
#define _SVR_COMPUTE_DEVICE_HPP_

#include "SVR/Interface.hpp"

namespace SVR
{
DECLARE_INTERFACE(ComputeKernel);

/**
 * Compute device.
 */
struct ComputeDevice: Interface
{
    virtual void runGlobalKernel1D(const ComputeKernelPtr &kernel, size_t globalWorkSize) = 0;
    virtual void runGlobalKernel2D(const ComputeKernelPtr &kernel, size_t globalWorkWidth, size_t globalWorkHeight) = 0;
    virtual void runGlobalKernel2D(const ComputeKernelPtr &kernel, size_t globalWorkWidth, size_t globalWorkHeight, size_t globalWorkDepth) = 0;
};

} // namespace SVR

#endif //_SVR_COMPUTE_DEVICE_HPP_

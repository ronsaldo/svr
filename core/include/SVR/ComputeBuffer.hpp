#ifndef _SVR_COMPUTE_BUFFER_HPP_
#define _SVR_COMPUTE_BUFFER_HPP_

#include "SVR/Interface.hpp"

namespace SVR
{

DECLARE_INTERFACE(ComputeBuffer);
DECLARE_INTERFACE(ComputeDevice);

/**
 * ComputeBuffer
 */
struct ComputeBuffer: Interface
{
    virtual void destroy() = 0;

    virtual void acquireFromRenderer(ComputeDevice *device) = 0;
    virtual void releaseFromRenderer(ComputeDevice *device) = 0;
};

} // namespace SVR

#endif //_SVR_COMPUTE_BUFFER_HPP_


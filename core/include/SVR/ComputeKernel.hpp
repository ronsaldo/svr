#ifndef _SVR_COMPUTE_KERNEL_HPP_
#define _SVR_COMPUTE_KERNEL_HPP_

#include <glm/glm.hpp>
#include "SVR/Interface.hpp"

namespace SVR
{

DECLARE_INTERFACE(ComputeKernel);
DECLARE_INTERFACE(ComputeBuffer);

/**
 * Computation kernel
 */
struct ComputeKernel: Interface
{
    virtual void setBufferArg(int arg, const ComputeBufferPtr &buffer) = 0;
    virtual void setIntArg(int arg, int value) = 0;

    virtual void setFloatArg(int arg, float value) = 0;
    virtual void setFloat2Arg(int arg, const glm::vec2 &value) = 0;
    virtual void setFloat4Arg(int arg, const glm::vec4 &value) = 0;

    virtual void setDoubleArg(int arg, double value) = 0;
    virtual void setDouble2Arg(int arg, const glm::dvec2 &value) = 0;
    virtual void setDouble4Arg(int arg, const glm::dvec4 &value) = 0;

};

} // namespace SVR

#endif //_SVR_COMPUTE_KERNEL_HPP_


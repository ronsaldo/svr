#ifndef _SVR_COMPUTE_PLATFORM_HPP_
#define _SVR_COMPUTE_PLATFORM_HPP_

#include "SVR/ComputeProgram.hpp"
#include "SVR/ComputeDevice.hpp"
#include "SVR/ComputeBuffer.hpp"

namespace SVR
{
DECLARE_INTERFACE(ComputePlatform);
DECLARE_INTERFACE(Texture1D);
DECLARE_INTERFACE(Texture2D);

/**
 * Heterogenous compute platform
 */
struct ComputePlatform
{
    virtual bool initialize(int argc, const char **argv) = 0;
    virtual void shutdown() = 0;

    virtual ComputeProgramPtr loadComputeProgramFromFile(const std::string &path) = 0;

    virtual ComputeBufferPtr createImageFromTexture1D(const Texture1DPtr &texture) = 0;
    virtual ComputeBufferPtr createImageFromTexture2D(const Texture2DPtr &texture) = 0;
};

ComputePlatformPtr createComputePlatform();

} // namespace SVR

#endif //_SVR_COMPUTE_PLATFORM_HPP_

#ifndef _SVR_COMPUTE_PLATFORM_HPP_
#define _SVR_COMPUTE_PLATFORM_HPP_

#include "SVR/ComputeProgram.hpp"
#include "SVR/ComputeDevice.hpp"
#include "SVR/ComputeBuffer.hpp"
#include "SVR/ComputeSampler.hpp"
#include "SVR/PixelFormat.hpp"

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

    virtual ComputeBufferPtr createImage1D(PixelFormat format, size_t width, const char *data=nullptr) = 0;
    virtual ComputeBufferPtr createImage2D(PixelFormat format, size_t width, size_t height, size_t rowPitch=0, const char *data=nullptr) = 0;
    virtual ComputeBufferPtr createImage3D(PixelFormat format, size_t width, size_t height, size_t depth, size_t rowPitch=0, size_t slicePitch = 0, const char *data=nullptr) = 0;

    virtual ComputeBufferPtr createImageFromTexture1D(const Texture1DPtr &texture) = 0;
    virtual ComputeBufferPtr createImageFromTexture2D(const Texture2DPtr &texture) = 0;

    virtual size_t getComputeDeviceCount() const = 0;
    virtual ComputeDevice *getComputeDevice(size_t index) = 0;

    virtual void beginCompute() = 0;
    virtual void endCompute() = 0;

    virtual ComputeSamplerPtr createNearestSampler() = 0;
    virtual ComputeSamplerPtr createLinearSampler() = 0;

};

ComputePlatformPtr createComputePlatform();

} // namespace SVR

#endif //_SVR_COMPUTE_PLATFORM_HPP_

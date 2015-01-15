#include <GL/glew.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>

#if defined(__WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <GL/glx.h>
#endif

#include <string.h>

#include <map>
#include <vector>

#include "SVR/ComputePlatform.hpp"
#include "SVR/LoadUtilities.hpp"
#include "SVR/Logging.hpp"
#include "SVR/Texture.hpp"

#if defined (__APPLE__) || defined(MACOSX)
    static const char* CL_GL_SHARING_EXT = "cl_APPLE_gl_sharing";
#else
    static const char* CL_GL_SHARING_EXT = "cl_khr_gl_sharing";
#endif

namespace SVR
{
DECLARE_CLASS(CLComputeKernel);

inline cl_channel_order computeMapPixelFormatOrder(PixelFormat format)
{
    switch(format)
    {
    case PixelFormat::L8:
        return CL_LUMINANCE;
    case PixelFormat::I8:
        return CL_INTENSITY;
    case PixelFormat::R8:
        return CL_R;
    case PixelFormat::RG8:
        return CL_RG;
    case PixelFormat::RGB8:
        return CL_RGB;
    case PixelFormat::RGBA8:
        return CL_RGBA;

    case PixelFormat::R32F:
        return CL_R;
    case PixelFormat::RG32F:
        return CL_RG;
    case PixelFormat::RGBA32F:
        return CL_RGBA;
    default:
        abort();
    }
}

inline cl_channel_type computeMapPixelFormatDataType(PixelFormat format)
{
    switch(format)
    {
    case PixelFormat::L8:
    case PixelFormat::I8:
    case PixelFormat::R8:
    case PixelFormat::RG8:
    case PixelFormat::RGB8:
    case PixelFormat::RGBA8:
        return CL_UNORM_INT8;

    case PixelFormat::R32F:
    case PixelFormat::RG32F:
    case PixelFormat::RGBA32F:
        return CL_FLOAT;

    default:
        abort();
    }
}

inline cl_image_format computeMapPixelFormat(PixelFormat format)
{
    cl_image_format clformat;
    clformat.image_channel_order = computeMapPixelFormatOrder(format);
    clformat.image_channel_data_type = computeMapPixelFormatDataType(format);
    return clformat;
}

/**
 * CLComputeDevice
 */
class CLComputeDevice: public ComputeDevice
{
public:
    CLComputeDevice(cl_context context, cl_device_id device);
    ~CLComputeDevice();

    bool initialize();
    void destroy();

    cl_device_id getHandle();
    cl_command_queue getCommandQueue();

    virtual void beginCompute();
    virtual void endCompute();

    virtual void runGlobalKernel1D(const ComputeKernelPtr &kernel, size_t globalWorkSize);
    virtual void runGlobalKernel2D(const ComputeKernelPtr &kernel, size_t globalWorkWidth, size_t globalWorkHeight);
    virtual void runGlobalKernel2D(const ComputeKernelPtr &kernel, size_t globalWorkWidth, size_t globalWorkHeight, size_t globalWorkDepth);

private:
    cl_context context;
    cl_device_id device;
    cl_command_queue commandQueue;
};

CLComputeDevice::CLComputeDevice(cl_context context, cl_device_id device)
    : context(context), device(device)
{
}

CLComputeDevice::~CLComputeDevice()
{
}

bool CLComputeDevice::initialize()
{
    commandQueue = clCreateCommandQueue(context, device, 0, nullptr);
    if(!commandQueue)
    {
        logError("Failed to create a command queue.");
        return false;
    }

    return true;
}

void CLComputeDevice::destroy()
{
    if(commandQueue)
        clReleaseCommandQueue(commandQueue);
    commandQueue = nullptr;
}

void CLComputeDevice::beginCompute()
{
}

void CLComputeDevice::endCompute()
{
    clFlush(commandQueue);
}

cl_device_id CLComputeDevice::getHandle()
{
    return device;
}

cl_command_queue CLComputeDevice::getCommandQueue()
{
    return commandQueue;
}

/**
 * OpenCL compute buffer
 */
class CLComputeBuffer: public ComputeBuffer
{
public:
    CLComputeBuffer(cl_context context, cl_mem mem);
    ~CLComputeBuffer();

    virtual void destroy();

    virtual void acquireFromRenderer(ComputeDevice *device);
    virtual void releaseFromRenderer(ComputeDevice *device);

    cl_mem getMem();

private:
    cl_context context;
    cl_mem mem;
};

CLComputeBuffer::CLComputeBuffer(cl_context context, cl_mem mem)
    : context(context), mem(mem)
{
}

CLComputeBuffer::~CLComputeBuffer()
{
    destroy();
}

void CLComputeBuffer::destroy()
{
    if(mem)
        clReleaseMemObject(mem);
    mem = nullptr;
}

void CLComputeBuffer::acquireFromRenderer(ComputeDevice *device)
{
    auto clDevice = static_cast<CLComputeDevice*> (device);
    clEnqueueAcquireGLObjects(clDevice->getCommandQueue(), 1, &mem, 0, 0, 0);
}

void CLComputeBuffer::releaseFromRenderer(ComputeDevice *device)
{
    auto clDevice = static_cast<CLComputeDevice*> (device);
    clEnqueueReleaseGLObjects(clDevice->getCommandQueue(), 1, &mem, 0, 0, 0);
}

cl_mem CLComputeBuffer::getMem()
{
    return mem;
}

/**
 * OpenCL compute kernel
 */
class CLComputeKernel: public ComputeKernel
{
public:
    CLComputeKernel(cl_context context, cl_kernel kernel);
    ~CLComputeKernel();

    virtual void destroy();

    virtual void setBufferArg(int arg, const ComputeBufferPtr &buffer);

    virtual void setIntArg(int arg, int value);

    virtual void setFloatArg(int arg, float value);
    virtual void setFloat2Arg(int arg, const glm::vec2 &value);
    virtual void setFloat4Arg(int arg, const glm::vec4 &value);

    virtual void setDoubleArg(int arg, double value);
    virtual void setDouble2Arg(int arg, const glm::dvec2 &value);
    virtual void setDouble4Arg(int arg, const glm::dvec4 &value);

    cl_kernel getKernel();

private:
    cl_context context;
    cl_kernel kernel;
};

CLComputeKernel::CLComputeKernel(cl_context context, cl_kernel kernel)
    : context(context), kernel(kernel)
{
}

CLComputeKernel::~CLComputeKernel()
{
    destroy();
}

void CLComputeKernel::destroy()
{
    if(kernel)
        clReleaseKernel(kernel);
    kernel = nullptr;
}

cl_kernel CLComputeKernel::getKernel()
{
    return kernel;
}

void CLComputeKernel::setBufferArg(int arg, const ComputeBufferPtr &buffer)
{
    auto clBuffer = std::static_pointer_cast<CLComputeBuffer> (buffer);
    auto clBufferHandle = clBuffer->getMem();
    clSetKernelArg(kernel, arg, sizeof(clBufferHandle), &clBufferHandle);
}

void CLComputeKernel::setIntArg(int arg, int value)
{
    clSetKernelArg(kernel, arg, sizeof(value), &value);
}

void CLComputeKernel::setFloatArg(int arg, float value)
{
    clSetKernelArg(kernel, arg, sizeof(value), &value);
}

void CLComputeKernel::setFloat2Arg(int arg, const glm::vec2 &value)
{
    clSetKernelArg(kernel, arg, sizeof(value), &value);
}

void CLComputeKernel::setFloat4Arg(int arg, const glm::vec4 &value)
{
    clSetKernelArg(kernel, arg, sizeof(value), &value);
}

void CLComputeKernel::setDoubleArg(int arg, double value)
{
    clSetKernelArg(kernel, arg, sizeof(value), &value);
}

void CLComputeKernel::setDouble2Arg(int arg, const glm::dvec2 &value)
{
    clSetKernelArg(kernel, arg, sizeof(value), &value);
}

void CLComputeKernel::setDouble4Arg(int arg, const glm::dvec4 &value)
{
    clSetKernelArg(kernel, arg, sizeof(value), &value);
}

void CLComputeDevice::runGlobalKernel1D(const ComputeKernelPtr &kernel, size_t globalWorkSize)
{
    auto clKernel = std::static_pointer_cast<CLComputeKernel> (kernel);
    clEnqueueNDRangeKernel(commandQueue, clKernel->getKernel(), 1, nullptr, &globalWorkSize, nullptr, 0, nullptr, nullptr);
}

void CLComputeDevice::runGlobalKernel2D(const ComputeKernelPtr &kernel, size_t globalWorkWidth, size_t globalWorkHeight)
{
    size_t sizes[] = {
        globalWorkWidth,
        globalWorkHeight
    };

    auto clKernel = std::static_pointer_cast<CLComputeKernel> (kernel);
    clEnqueueNDRangeKernel(commandQueue, clKernel->getKernel(), 2, nullptr, sizes, nullptr, 0, nullptr, nullptr);
}

void CLComputeDevice::runGlobalKernel2D(const ComputeKernelPtr &kernel, size_t globalWorkWidth, size_t globalWorkHeight, size_t globalWorkDepth)
{
    size_t sizes[] = {
        globalWorkWidth,
        globalWorkHeight,
        globalWorkDepth,
    };

    auto clKernel = std::static_pointer_cast<CLComputeKernel> (kernel);
    clEnqueueNDRangeKernel(commandQueue, clKernel->getKernel(), 2, nullptr, sizes, nullptr, 0, nullptr, nullptr);
}

/**
 * OpenCL compute program.
 */
class CLComputeProgram: public ComputeProgram
{
public:
    CLComputeProgram(cl_context context, CLComputeDevice *device, cl_program program, const std::string &name);
    ~CLComputeProgram();

    virtual bool build(const std::string &options);
    virtual void destroy();

    virtual ComputeKernelPtr createKernel(const std::string &name);

    cl_program getHandle();

private:
    cl_context context;
    CLComputeDevice *device;
    cl_program program;
    std::string name;
    std::map<std::string, CLComputeKernelPtr> kernels;
};

CLComputeProgram::CLComputeProgram(cl_context context, CLComputeDevice *device, cl_program program, const std::string &name)
    : context(context), device(device), program(program), name(name)
{
}

CLComputeProgram::~CLComputeProgram()
{
    destroy();
}

bool CLComputeProgram::build(const std::string &options)
{
    char buffer[4096];

    cl_device_id dev = device->getHandle();
    auto err = clBuildProgram(program, 1, &dev, options.c_str(), nullptr, nullptr);
    if(err != 0)
    {
        logError("Failed to build program");
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, sizeof(buffer)-1, buffer, nullptr);
        fprintf(stderr, "Build log for '%s':\n%s\n", name.c_str(), buffer);
        return false;
    }

    return true;
}

void CLComputeProgram::destroy()
{
    for (auto &nameKernel: kernels)
        nameKernel.second->destroy();

    if(program)
        clReleaseProgram(program);

    program = nullptr;
}

ComputeKernelPtr CLComputeProgram::createKernel(const std::string &name)
{
    auto it = kernels.find(name);
    if(it != kernels.end())
        return it->second;

    cl_int err;
    auto kernel = clCreateKernel(program, name.c_str(), &err);
    if(err)
    {
        logError("Failed to create kernel");
        return ComputeKernelPtr();
    }

    auto kernelWrapper = std::make_shared<CLComputeKernel> (context, kernel);
    kernels.insert(std::make_pair(name, kernelWrapper));
    return kernelWrapper;
}

cl_program CLComputeProgram::getHandle()
{
    return program;
}

/**
 * OpenCL compute platform.
 */
class CLComputePlatform: public ComputePlatform
{
public:
    CLComputePlatform();
    ~CLComputePlatform();

    virtual bool initialize(int argc, const char **argv);
    virtual void shutdown();

    virtual ComputeProgramPtr loadComputeProgramFromFile(const std::string &path);

    virtual ComputeBufferPtr createImage1D(PixelFormat format, size_t width, const char *data);
    virtual ComputeBufferPtr createImage2D(PixelFormat format, size_t width, size_t height, size_t rowPitch, const char *data);
    virtual ComputeBufferPtr createImage3D(PixelFormat format, size_t width, size_t height, size_t depth, size_t rowPitch, size_t slicePitch, const char *data);

    virtual ComputeBufferPtr createImageFromTexture1D(const Texture1DPtr &texture);
    virtual ComputeBufferPtr createImageFromTexture2D(const Texture2DPtr &texture);

    virtual size_t getComputeDeviceCount() const;
    virtual ComputeDevice *getComputeDevice(size_t index);

    virtual void beginCompute();
    virtual void endCompute();

private:
    bool createContext();
    bool initializeDevices();
    bool checkOpenGLSharing();
    bool isExtensionSupported(const std::string &extension);

    clGetGLContextInfoKHR_fn clGetGLContextInfoKHR;

    cl_platform_id platform;
    cl_context context;
    std::string extensionString;

    std::vector<CLComputeDevice> devices;
    std::vector<cl_device_id> devicesIDs;
};

ComputePlatformPtr createComputePlatform()
{
    return std::make_shared<CLComputePlatform> ();
}

CLComputePlatform::CLComputePlatform()
{
}

CLComputePlatform::~CLComputePlatform()
{
}

bool CLComputePlatform::initialize(int argc, const char **argv)
{
    if(!createContext())
        return false;

    if(!initializeDevices())
        return false;

    return true;
}

bool CLComputePlatform::checkOpenGLSharing()
{
    // Get the devices.
    cl_uint numdevices;
    cl_device_id deviceId;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &deviceId, &numdevices);
    if(numdevices == 0)
    {
        logError("Failed to find an OpenCL GPU device");
        return false;
    }

    // Get string containing supported device extensions
    char extensionStringBuffer[2048];
    size_t extensionStringSize;
    auto err = clGetDeviceInfo(deviceId, CL_DEVICE_EXTENSIONS, sizeof(extensionStringBuffer), extensionStringBuffer, &extensionStringSize);
    if(err)
    {
        logError("Failed to query extension string");
        return false;
    }

    extensionString = std::string(extensionStringBuffer, extensionStringBuffer + extensionStringSize);

    // Search for GL support in extension string (space delimited)
    if(!isExtensionSupported(CL_GL_SHARING_EXT))
    {
        logError("OpenCL <-> OpenGL sharing support not found");
        return false;
    }

    // Load some extensions.
    clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddressForPlatform(platform, "clGetGLContextInfoKHR");
    if(!clGetGLContextInfoKHR)
    {
        logError("Failed to load clGetGLContextInfoKHR function address");
        return false;
    }

    return true;
}

bool CLComputePlatform::isExtensionSupported(const std::string &extension)
{
    return extensionString.find(extension) != std::string::npos;
}

void CLComputePlatform::shutdown()
{
    for(auto &device : devices)
        device.destroy();
    clReleaseContext(context);
}

bool CLComputePlatform::createContext()
{
    // Get the platform id.
    cl_uint numplatforms;
    clGetPlatformIDs(1, &platform, &numplatforms);
    if(numplatforms == 0)
    {
        logError("Failed to find an OpenCL platform");
        return false;
    }

    // Check for OpenGL sharing
    if(!checkOpenGLSharing())
        return false;

    // The opencl context properties
    cl_context_properties properties[] = {
#if defined(_WIN32)
        CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), // WGL Context
        CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), // WGL HDC
#elif defined(__linux__)
        CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(), // GLX Context
        CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
#else
#error Unsupported platform for OpenCL/OpenGL sharing
#endif
        CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
        // OpenCL platform
        0
    };

    // Find CL capable devices in the current GL context
    cl_device_id devices[32];
    size_t size;
    clGetGLContextInfoKHR(properties, CL_DEVICES_FOR_GL_CONTEXT_KHR, sizeof(devices), devices, &size);
    
    // Create a context using the supported devices
    int numdevices = size / sizeof(devices[0]);
    context = clCreateContext(properties, numdevices, devices, nullptr, 0, 0);
    if(!context)
    {
        logError("Failed to create OpenL context");
        return false;
    }

    // Create the device wrappers.
    this->devicesIDs = std::vector<cl_device_id> (devices,  devices + numdevices);
    this->devices.reserve(numdevices);
    for(int i = 0; i < numdevices; ++i)
        this->devices.push_back(CLComputeDevice(context, devices[i]));

    return true;
}

bool CLComputePlatform::initializeDevices()
{
    for(auto &device : devices)
    {
        if(!device.initialize())
            return false;
    }

    return true;
}

ComputeProgramPtr CLComputePlatform::loadComputeProgramFromFile(const std::string &path)
{
    // Load the OpenCL C source code
    std::vector<char> sourceCode;
    if(!loadTextFileInto(path, sourceCode))
        return ComputeProgramPtr();

    const char *sourceCodePtr = &sourceCode[0];
    auto program = clCreateProgramWithSource(context, 1, &sourceCodePtr, nullptr, nullptr);
    if(!program)
    {
        logError("Failed to create program with source");
        return ComputeProgramPtr();
    }

    return std::make_shared<CLComputeProgram> (context, &devices[0], program, path);
}

ComputeBufferPtr CLComputePlatform::createImage1D(PixelFormat format, size_t width, const char *data)
{
    auto imageFormat = computeMapPixelFormat(format);
    cl_image_desc desc;
    memset(&desc, 0, sizeof(desc));
    desc.image_type = CL_MEM_OBJECT_IMAGE1D;
    desc.image_width = width;
    desc.image_height = 1;
    desc.image_depth = 1;
    desc.image_array_size = 1;
    desc.image_row_pitch = 0;
    desc.image_slice_pitch = 0;

    cl_mem_flags flags = CL_MEM_READ_WRITE;
    if(data)
        flags |=  CL_MEM_COPY_HOST_PTR;

    cl_int error;
    auto buffer = clCreateImage(context, flags, &imageFormat, &desc, (void*)data, &error);
    if(!buffer || error)
    {
        logError("Failed to allocate 1D image buffer");
        return ComputeBufferPtr();
    }

    return std::make_shared<CLComputeBuffer> (context, buffer);
}

ComputeBufferPtr CLComputePlatform::createImage2D(PixelFormat format, size_t width, size_t height, size_t rowPitch, const char *data)
{
    auto imageFormat = computeMapPixelFormat(format);
    cl_image_desc desc;
    memset(&desc, 0, sizeof(desc));
    desc.image_type = CL_MEM_OBJECT_IMAGE1D;
    desc.image_width = width;
    desc.image_height = height;
    desc.image_depth = 1;
    desc.image_array_size = 1;
    desc.image_row_pitch = rowPitch;
    desc.image_slice_pitch = 0;

    cl_mem_flags flags = CL_MEM_READ_WRITE;
    if(data)
        flags |=  CL_MEM_COPY_HOST_PTR;

    cl_int error;
    auto buffer = clCreateImage(context, flags, &imageFormat, &desc, (void*)data, &error);
    if(!buffer || error)
    {
        logError("Failed to allocate 2D image buffer");
        return ComputeBufferPtr();
    }

    return std::make_shared<CLComputeBuffer> (context, buffer);
}

ComputeBufferPtr CLComputePlatform::createImage3D(PixelFormat format, size_t width, size_t height, size_t depth, size_t rowPitch, size_t slicePitch, const char *data)
{
    auto imageFormat = computeMapPixelFormat(format);
    cl_image_desc desc;
    memset(&desc, 0, sizeof(desc));
    desc.image_type = CL_MEM_OBJECT_IMAGE3D;
    desc.image_width = width;
    desc.image_height = height;
    desc.image_depth = depth;
    desc.image_array_size = 1;
    desc.image_row_pitch = rowPitch;
    desc.image_slice_pitch = slicePitch;

    cl_mem_flags flags = CL_MEM_READ_WRITE;
    if(data)
        flags |=  CL_MEM_COPY_HOST_PTR;

    cl_int error;
    auto buffer = clCreateImage(context, flags, &imageFormat, &desc, (void*)data, &error);
    if(!buffer || error)
    {
        logError("Failed to allocate 3D image buffer");
        return ComputeBufferPtr();
    }

    return std::make_shared<CLComputeBuffer> (context, buffer);
}

ComputeBufferPtr CLComputePlatform::createImageFromTexture1D(const Texture1DPtr &texture)
{
    auto handle = (GLuint)(size_t)texture->getHandle();
    cl_int err;
    auto image = clCreateFromGLTexture(context, CL_MEM_READ_WRITE, GL_TEXTURE_1D, 0, handle, &err);
    if(!image)
    {
        logError("Failed to create image from OpenGL texture 1D");
        return ComputeBufferPtr();
    }

    return std::make_shared<CLComputeBuffer> (context, image);
}

ComputeBufferPtr CLComputePlatform::createImageFromTexture2D(const Texture2DPtr &texture)
{
    
    auto handle = (GLuint)(size_t)texture->getHandle();
    cl_int err;
    auto image = clCreateFromGLTexture(context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, handle, &err);
    if(!image)
    {
        logError("Failed to create image from OpenGL texture 2D");
        return ComputeBufferPtr();
    }

    return std::make_shared<CLComputeBuffer> (context, image);
}

size_t CLComputePlatform::getComputeDeviceCount() const
{
    return devices.size();
}

ComputeDevice *CLComputePlatform::getComputeDevice(size_t index)
{
    return &devices[index];
}

void CLComputePlatform::beginCompute()
{
    for(auto &device : devices)
        device.beginCompute();
}

void CLComputePlatform::endCompute()
{
    for(auto &device : devices)
        device.endCompute();
}

} // namespace SVR

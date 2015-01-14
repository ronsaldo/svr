#include <GL/glew.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>

#if defined(__WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <GL/glx.h>
#endif

#include <map>
#include <vector>

#include "SVR/ComputePlatform.hpp"
#include "SVR/LoadUtilities.hpp"
#include "SVR/Logging.hpp"

#if defined (__APPLE__) || defined(MACOSX)
    static const char* CL_GL_SHARING_EXT = "cl_APPLE_gl_sharing";
#else
    static const char* CL_GL_SHARING_EXT = "cl_khr_gl_sharing";
#endif

namespace SVR
{
DECLARE_CLASS(CLComputeKernel);

/**
 * CLComputeDevice
 */
class CLComputeDevice
{
public:
    CLComputeDevice(cl_context context, cl_device_id device);
    ~CLComputeDevice();

    bool initialize();
    void destroy();

    cl_device_id getHandle();

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
    destroy();
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

cl_device_id CLComputeDevice::getHandle()
{
    return device;
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

/**
 * OpenCL compute program.
 */
class CLComputeProgram: public ComputeProgram
{
public:
    CLComputeProgram(cl_context context, cl_program program);
    ~CLComputeProgram();

    virtual bool build(const std::string &options);
    virtual void destroy();

    virtual ComputeKernelPtr createKernel(const std::string &name);

    cl_program getHandle();

private:
    cl_context context;
    cl_program program;
    std::map<std::string, CLComputeKernelPtr> kernels;
};

CLComputeProgram::CLComputeProgram(cl_context context, cl_program program)
    : context(context), program(program)
{
}

CLComputeProgram::~CLComputeProgram()
{
    destroy();
}

bool CLComputeProgram::build(const std::string &options)
{
    return true;
}

void CLComputeProgram::destroy()
{
    if(program)
        clReleaseProgram(program);
    program = nullptr;
}

ComputeKernelPtr CLComputeProgram::createKernel(const std::string &name)
{
    auto it = kernels.find(name);
    if(it != kernels.end())
        return it->second;

    auto kernel = clCreateKernel(program, name.c_str(), nullptr);
    if(!kernel)
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

    return std::make_shared<CLComputeProgram> (context, program);
}

} // namespace SVR

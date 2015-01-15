#include <string.h>

#include "Application.hpp"

namespace SVR
{

Application::Application()
    : isQuitting(false), window(nullptr), glContext(nullptr)
{
    screenWidth = 640;
    screenHeight = 480;
    fovy = 60.0;
    cubeFile = nullptr;
    gammaCorrection = 2.2;

    // TODO initialize this.
    cubeImageBox;
    ubeViewRegion;
    lengthScale;

    minNumberOfSamples;
    maxNumberOfSamples;
    lengthSamplingFactor;

    filterMinValue;
    filterMaxValue;
}

Application::~Application()
{
}

int Application::main(int argc, const char **argv)
{
    if(!initialize(argc, argv))
        return false;

    mainLoop();
    shutdown();

    return 0;
}

void Application::quit()
{
    isQuitting = true;
}

bool Application::initialize(int argc, const char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    if(!parseCommandLine(argc, argv))
        return false;

    window = SDL_CreateWindow("SVR",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                screenWidth, screenHeight,  SDL_WINDOW_OPENGL);
    if(!window)
        fatalError("Failed to create window");

    glContext = SDL_GL_CreateContext(window);
    if(!glContext)
        fatalError("Failed to create window");

    // Create the renderer.
    renderer = createRenderer();
    if(!renderer->initialize(argc, argv))
        fatalError("Failed to initialize the renderer");

    // Activate the opengl context.
    if(SDL_GL_MakeCurrent(window, glContext))
        fatalError("Failed to active the opengl context");

    if(!initializeTextures())
        fatalError("Failed to initialize textures");

    // Create the compute platform.
    computePlatform = createComputePlatform();
    if(!computePlatform->initialize(argc, argv))
        fatalError("Failed to initialize compute platform");

    if(!initializeComputation())
        fatalError("Failed to initialize computation");

    // Initialize the sccene.
    if(!initializeScene())
        fatalError("Failed to initialize the scene");

    return true;
}

bool Application::parseCommandLine(int argc, const char **argv)
{
    for(int i = 1; i < argc; ++i)
    {
        if(!strcmp(argv[i], "-sw") && argv[++i])
        {
            screenWidth = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "-sh") && argv[++i])
        {
            screenHeight = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "-fovy") && argv[++i])
        {
            fovy = atof(argv[i]);
        }
        else if(!strcmp(argv[i], "-cube") && argv[++i])
        {
            cubeFileName = argv[i];
        }
    }

    return !cubeFileName.empty();
}

bool Application::initializeTextures()
{
    colorBuffer = renderer->createTexture2D(screenWidth, screenHeight, PixelFormat::RGBA32F);
    colorBuffer->allocateInDevice();

    return colorBuffer.get();
}

void Application::setColorMap(const ColorMap &newColorMap)
{
    colorMap = newColorMap;

    if(colorMapTexture)
    {
        computeColorMap->destroy();
        //colorMapTexture->destroy();
    }

    colorMapTexture = renderer->createTexture1D(colorMap.colors.size(), PixelFormat::RGBA32F);
    colorMapTexture->upload(PixelFormat::RGBA32F, sizeof(glm::vec4)*colorMap.colors.size(),  &colorMap.colors[0]);

    computeColorMap = computePlatform->createImageFromTexture1D(colorMapTexture);
}

bool Application::initializeComputation()
{
    // Raycast program
    raycastProgram = computePlatform->loadComputeProgramFromFile("data/kernels/raycast.cl");
    if(!raycastProgram->build())
        return false;

    // Cube mapping float
    cubeMappingsFloatProgram = computePlatform->loadComputeProgramFromFile("data/kernels/cubeMappingsFloat.cl");
    if(!cubeMappingsFloatProgram->build())
        return false;

    // Cube mapping double
    cubeMappingsDoubleProgram = computePlatform->loadComputeProgramFromFile("data/kernels/cubeMappingsDouble.cl");
    if(!cubeMappingsDoubleProgram->build())
        return false;

    testProgram = computePlatform->loadComputeProgramFromFile("data/kernels/test.cl");
    if(!testProgram->build())
        return false;

    computeColorBuffer = computePlatform->createImageFromTexture2D(colorBuffer);
    return true;
}

bool Application::initializeScene()
{
    // Create the camera.
    camera = std::make_shared<Camera> ();
    camera->setPosition(glm::vec3(0.0, 0.0, 3.0));

    // Load the image cube.
    cubeFile = FitsFile::open(cubeFileName.c_str(), false);
    printf("Opened cube of size: %d %d %d\n", (int)cubeFile->getWidth(), (int)cubeFile->getHeight(), (int)cubeFile->getDepth());

    // Allocate space for the mapped fits
    size_t wholeSize = cubeFile->getWidth()*cubeFile->getHeight()*cubeFile->getDepth();
    std::unique_ptr<uint8_t[]> wholeData(new uint8_t[wholeSize]);

    // Map the cube.
    mapFitsInto(LinearMapping(), cubeFile, wholeData.get());

    // Create the compute buffer.
    computeCubeBuffer = computePlatform->createImage3D(PixelFormat::L8, cubeFile->getWidth(), cubeFile->getHeight(), cubeFile->getDepth(), cubeFile->getWidth(), cubeFile->getWidth()*cubeFile->getDepth(), (char*)wholeData.get());

    // Set the color map.
    setColorMap(ColorMap::gray());

    return true;
}

void Application::shutdown()
{
    computeCubeBuffer->destroy();
    computeColorBuffer->destroy();
    testProgram->destroy();
    raycastProgram->destroy();
    cubeMappingsFloatProgram->destroy();
    cubeMappingsDoubleProgram->destroy();
    computeColorMap->destroy();

    computePlatform->shutdown();
    renderer->shutdown();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Application::mainLoop()
{
    while(!isQuitting)
    {
        processEvents();
        render();
    }
}

void Application::processEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_KEYDOWN:
            onKeyDown(event.key);
            break;
        case SDL_QUIT:
            quit();
            break;
        }
    }
}

void Application::raycast()
{
    // Transformed camera
    FrustumCorners transformedFrustum;
    camera->getWorldFrustumCorners(transformedFrustum);

    auto device = computePlatform->getComputeDevice(0);

    // Acquire shared resources
    renderer->beginCompute();
    computePlatform->beginCompute();
    computeColorBuffer->acquireFromRenderer(device);
    computeColorMap->acquireFromRenderer(device);

    // Setup the kernel
    auto kernel = testProgram->createKernel("raycastVolume");

    kernel->setBufferArg(0, computeCubeBuffer);
    kernel->setBufferArg(1, computeColorBuffer);

    // Pass the camera    
    for(int i = 0; i < 8; ++i)
        kernel->setFloat4Arg(2 + i, transformedFrustum[i]);

    // Cube parameters
    kernel->setFloat4Arg(9, cubeImageBox.min);
    kernel->setFloat4Arg(10, cubeImageBox.max);
    kernel->setFloat4Arg(11, cubeViewRegion.min);
    kernel->setFloat4Arg(12, cubeViewRegion.max);
    kernel->setFloatArg(13, lengthScale);

    // Sampling
    kernel->setIntArg(14, minNumberOfSamples);
    kernel->setIntArg(15, maxNumberOfSamples);
    kernel->setFloatArg(16, lengthSamplingFactor);
    kernel->setSamplerArg(17, currentSampler);

    // Color mapping
    kernel->setBufferArg(18, computeColorMap);
    kernel->setFloatArg(19, filterMinValue);
    kernel->setFloatArg(20, filterMaxValue);

    // Color correction
    kernel->setFloatArg(21, 1.0 / gammaCorrection);

    // Run the rendering kernel
    device->runGlobalKernel2D(kernel, colorBuffer->getWidth(), colorBuffer->getHeight());

    // Release the shared resources.
    computeColorBuffer->releaseFromRenderer(device);
    computePlatform->endCompute();
    renderer->endCompute();
}

void Application::render()
{
    if(SDL_GL_MakeCurrent(window, glContext))
        return;

    // Update the screen size.
    auto extent = glm::vec2(screenWidth, screenHeight);
    float aspect = extent.x/extent.y;
    camera->perspective(fovy, aspect, 0.01, 100.0);

    // Perform the rendering
    raycast();

    renderer->setScreenSize(extent);

    // Clear the window.
    renderer->clearColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
    renderer->clear();

    renderer->setTexture(colorBuffer);
    renderer->drawRectangle(glm::vec2(0.0, 0.0), extent);

    renderer->setColor(glm::vec4(0.0, 1.0, 0.0, 0.0));
    renderer->drawLine(glm::vec2(0.0, 200.0), glm::vec2(200.0, 200.0));

    renderer->setColor(glm::vec4(0.0, 0.0, 1.0, 0.0));
    renderer->drawTriangle(glm::vec2(200.0, 100.0), glm::vec2(400.0, 250.0), glm::vec2(200.0, 400.0));

    renderer->setColor(glm::vec4(1.0, 0.0, 0.0, 0.0));
    renderer->drawTriangle(glm::vec2(400.0, 100.0), glm::vec2(600.0, 250.0), glm::vec2(400.0, 400.0));

    renderer->flushCommands();
    SDL_GL_SwapWindow(window);
}

void Application::onKeyDown(const SDL_KeyboardEvent &event)
{
    switch(event.keysym.sym)
    {
    case SDLK_ESCAPE:
        quit();
        break;
    }
}

void Application::onKeyUp(const SDL_KeyboardEvent &event)
{
}

} // namespace SVR



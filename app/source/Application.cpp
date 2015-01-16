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

    cubeViewRegion = AABox(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0));
    lengthScale = 4.0;

    minNumberOfSamples = 20;
    lengthSamplingFactor = 1.5;

    filterMinValue = -1.0;
    filterMaxValue = 2.0;

    colorMapName = "sls";
    dataScale = DataScale::Linear;
    initializeDictionaries();
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

    if(!initializeUI())
        fatalError("Failed to initialize the UI");

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
        else if(!strcmp(argv[i], "-colormap") && argv[++i])
        {
            colorMapName = argv[i];
        }
        else if(!strcmp(argv[i], "-datascale") && argv[++i])
        {
            setDataScaleNamed(argv[i]);
        }
    }

    return !cubeFileName.empty();
}

void Application::setDataScale(DataScale newDataScale)
{
    dataScale = newDataScale;
    // TODO: Perform the new mapping.
}

void Application::setDataScaleNamed(const std::string &name)
{
    auto it = dataScaleNameMap.find(name);
    if(it != dataScaleNameMap.end())
        setDataScale(it->second);
    else
        setDataScale(DataScale::Linear);
}

bool Application::initializeTextures()
{
    colorBuffer = renderer->createTexture2D(screenWidth, screenHeight, PixelFormat::RGBA32F);
    colorBuffer->allocateInDevice();

    return colorBuffer.get();
}

void Application::initializeDictionaries()
{
    colorMapNameDictionary["red"] = ColorMap::red();
    colorMapNameDictionary["green"] = ColorMap::green();
    colorMapNameDictionary["blue"] = ColorMap::blue();
    colorMapNameDictionary["rainbow"] = ColorMap::rainbow();
    colorMapNameDictionary["sls"] = ColorMap::sls();
    colorMapNameDictionary["haze"] = ColorMap::haze();

    dataScaleNameMap["linear"] = DataScale::Linear;
    dataScaleNameMap["log"] = DataScale::Log;
}

void Application::setColorMapNamed(const std::string &name)
{
    auto it = colorMapNameDictionary.find(name);
    if(it != colorMapNameDictionary.end())
        setColorMap(it->second);
    else
        setColorMap(ColorMap::sls());

}

void Application::setColorMap(const ColorMapPtr &newColorMap)
{
    colorMap = newColorMap;

    if(colorMapTexture)
    {
        computeColorMap->destroy();
        //colorMapTexture->destroy();
    }

    colorMapTexture = renderer->createTexture1D(colorMap->colors.size(), PixelFormat::RGBA32F);
    colorMapTexture->upload(PixelFormat::RGBA32F, sizeof(glm::vec4)*colorMap->colors.size(), &colorMap->colors[0]);

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

    nearestSampler = computePlatform->createNearestSampler();
    linearSampler = computePlatform->createLinearSampler();
    currentSampler = linearSampler;
    if(!nearestSampler || !linearSampler)
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

    performScaleMapping();

    // Set the color map.
    setColorMapNamed(colorMapName);

    return true;
}

bool Application::initializeUI()
{
    colorBarWidget = std::make_shared<ColorBarWidget> ();
    colorBarWidget->setPosition(glm::vec2(600, 0.0));
    colorBarWidget->setSize(glm::vec2(40, 480));
    colorBarWidget->setGradient(colorMapTexture);

    return true;
}

void Application::performScaleMapping()
{
    // Allocate space for the mapped fits
    size_t wholeSize = cubeFile->getWidth()*cubeFile->getHeight()*cubeFile->getDepth();
    std::unique_ptr<uint8_t[]> wholeData(new uint8_t[wholeSize]);

    // Map the cube.
    switch(dataScale)
    {
    case DataScale::Linear:
        mapFitsInto(LinearMapping(), cubeFile, wholeData.get());
        break;
    case DataScale::Log:
        mapFitsInto(LogMapping(), cubeFile, wholeData.get());
        break;
    default:
        break;
    }

    // Create the compute buffer.
    if(!computeCubeBuffer)
        computeCubeBuffer = computePlatform->createImage3D(PixelFormat::L8, cubeFile->getWidth(), cubeFile->getHeight(), cubeFile->getDepth(), cubeFile->getWidth(), cubeFile->getWidth()*cubeFile->getHeight(), (char*)wholeData.get());

    // TODO: Upload the new version of the data
}

void Application::shutdown()
{
    computeCubeBuffer->destroy();
    computeColorBuffer->destroy();
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
    unsigned int lastFpsUpdateTime, fpsCount;
    unsigned int oldTime, newTime;
    char titleBuffer[256];
    oldTime = SDL_GetTicks();
    lastFpsUpdateTime = oldTime;
    fpsCount = 0;

	while (!isQuitting)
	{
        // Process the events
		processEvents();

        // Compute the time delta.
        newTime = SDL_GetTicks();
        float delta = (newTime - oldTime)*0.001f;

        // Run game logic.
        update(delta);

        // Display the frame.
		render();

        // Count the FPS.
        ++fpsCount;
        if(newTime - lastFpsUpdateTime >= 1000)
        {
            sprintf(titleBuffer, "SVR - %02d FPS", fpsCount);
            SDL_SetWindowTitle(window, titleBuffer);
            fpsCount = 0;
            lastFpsUpdateTime = newTime;
        }

        // Store the new old time.
        oldTime = newTime;

		//SDL_Delay(10);
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
        case SDL_KEYUP:
            onKeyUp(event.key);
            break;
        case SDL_QUIT:
            quit();
            break;
        }
    }
}

void Application::update(float delta)
{
    const float AngularSpeed = 50.0f;
    const float LinearSpeed = 2.0f;

    cameraAngle += cameraAngularVelocity*(AngularSpeed*delta);
    auto newRotation = glm::conjugate(xyzEulerToQuaternion(degreesToRadians(cameraAngle)));
    camera->setOrientation(newRotation);

    auto oldPosition = camera->getPosition();
    camera->setPosition(oldPosition + glm::rotate(newRotation, cameraVelocity*(delta*LinearSpeed)));
}

void Application::computeCubeImageBox()
{
    auto cubeExtent = glm::vec3(cubeFile->getWidth(), cubeFile->getHeight(), cubeFile->getDepth());
    auto maxAxis = std::max(cubeExtent.x, std::max(cubeExtent.y, cubeExtent.z));
    auto cubeHalfExtent = cubeExtent * float(lengthScale * 0.5 / maxAxis);
	cubeImageBox = AABox(-cubeHalfExtent, cubeHalfExtent);
}

void Application::raycast()
{
    // Compute the viewed region.
    computeCubeImageBox();

    // Transformed camera
    FrustumCorners transformedFrustum;
    camera->getWorldFrustumCorners(transformedFrustum);

    auto device = computePlatform->getComputeDevice(0);

    // Compute the max number of samples
    auto w = cubeFile->getWidth();
    auto h = cubeFile->getHeight();
    auto d = cubeFile->getDepth();
    maxNumberOfSamples = ceil(sqrt(w*w + h*h + d*d) * lengthSamplingFactor);

    // Acquire shared resources
    renderer->beginCompute();
    computePlatform->beginCompute();
    computeColorBuffer->acquireFromRenderer(device);
    computeColorMap->acquireFromRenderer(device);

    // Setup the kernel
    auto kernel = raycastProgram->createKernel("raycastVolume");

    kernel->setBufferArg(0, computeCubeBuffer);
    kernel->setBufferArg(1, computeColorBuffer);

    // Pass the camera    
    for(int i = 0; i < 8; ++i)
    {
        //auto p = transformedFrustum[i];
        //printf("Frustum %d %f %f %f:%f\n", i, p.x, p.y, p.z, p.w);
        kernel->setFloat4Arg(2 + i, transformedFrustum[i]);
    }

    // Cube parameters
    kernel->setFloat4Arg(10, glm::vec4(cubeImageBox.min, 0.0));
    kernel->setFloat4Arg(11, glm::vec4(cubeImageBox.max,0.0));
    kernel->setFloat4Arg(12, glm::vec4(cubeViewRegion.min, 0.0));
    kernel->setFloat4Arg(13, glm::vec4(cubeViewRegion.max, 0.0));
    kernel->setFloatArg(14, lengthScale);

    // Sampling
    kernel->setIntArg(15, minNumberOfSamples);
    kernel->setIntArg(16, maxNumberOfSamples);
    kernel->setFloatArg(17, lengthSamplingFactor);
    kernel->setSamplerArg(18, currentSampler);

    // Color mapping
    kernel->setBufferArg(19, computeColorMap);
    kernel->setFloatArg(20, filterMinValue);
    kernel->setFloatArg(21, filterMaxValue);

    // Color correction
    kernel->setFloatArg(22, 1.0 / gammaCorrection);

    // Run the rendering kernel
    //printf("Render frame %d %d\n", minNumberOfSamples, maxNumberOfSamples);
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


    colorBarWidget->draw(renderer);

    /*renderer->setColor(glm::vec4(0.0, 1.0, 0.0, 0.0));
    renderer->drawLine(glm::vec2(0.0, 200.0), glm::vec2(200.0, 200.0));

    renderer->setColor(glm::vec4(0.0, 0.0, 1.0, 0.0));
    renderer->drawTriangle(glm::vec2(200.0, 100.0), glm::vec2(400.0, 250.0), glm::vec2(200.0, 400.0));

    renderer->setColor(glm::vec4(1.0, 0.0, 0.0, 0.0));
    renderer->drawTriangle(glm::vec2(400.0, 100.0), glm::vec2(600.0, 250.0), glm::vec2(400.0, 400.0));*/

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
    case SDLK_w:
        cameraVelocity.z = -1;
        break;
    case SDLK_s:
        cameraVelocity.z = 1;
        break;
    case SDLK_a:
        cameraVelocity.x = -1;
        break;
    case SDLK_d:
        cameraVelocity.x = 1;
        break;
    case SDLK_SPACE:
        cameraVelocity.y = 1;
        break;
    case SDLK_LCTRL:
        cameraVelocity.y = -1;
        break;
    case SDLK_LEFT:
        cameraAngularVelocity.y = -1;
        break;
    case SDLK_RIGHT:
        cameraAngularVelocity.y = 1;
        break;
    case SDLK_UP:
        cameraAngularVelocity.x = -1;
        break;
    case SDLK_DOWN:
        cameraAngularVelocity.x = 1;
        break;
    }
}

void Application::onKeyUp(const SDL_KeyboardEvent &event)
{
    switch(event.keysym.sym)
    {
    case SDLK_w:
        if(cameraVelocity.z < 0)
            cameraVelocity.z = 0;
        break;
    case SDLK_s:
        if(cameraVelocity.z > 0)
            cameraVelocity.z = 0;
        break;
    case SDLK_a:
        if(cameraVelocity.x < 0)
            cameraVelocity.x = 0;
        break;
    case SDLK_d:
        if(cameraVelocity.x > 0)
            cameraVelocity.x = 0;
        break;
    case SDLK_SPACE:
        if(cameraVelocity.y > 0)
            cameraVelocity.y = 0;
        break;
    case SDLK_LCTRL:
        if(cameraVelocity.y < 0)
            cameraVelocity.y = 0;
        break;
    case SDLK_LEFT:
        if(cameraAngularVelocity.y < 0)
            cameraAngularVelocity.y = 0;
        break;
    case SDLK_RIGHT:
        if(cameraAngularVelocity.y > 0)
            cameraAngularVelocity.y = 0;
        break;
    case SDLK_UP:
        if(cameraAngularVelocity.x < 0)
            cameraAngularVelocity.x = 0;
        break;
    case SDLK_DOWN:
        if(cameraAngularVelocity.x > 0)
            cameraAngularVelocity.x = 0;
        break;
    }
}

} // namespace SVR



#include <string.h>

#include "Application.hpp"
#include "SVR/DockingLayout.hpp"

namespace SVR
{

Application::Application()
    : isQuitting(false), window(nullptr), glContext(nullptr)
{
    screenWidth = 640;
    screenHeight = 480;
    fullscreen = false;
    fovy = 60.0;
    cubeFile = nullptr;
    gammaCorrection = 2.2;

    cubeViewRegion = AABox(glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0));
    lengthScale = 4.0;

    minNumberOfSamples = 20;
    lengthSamplingFactor = 1.5;
    sampleColorIntensity = glm::vec4(1.0, 1.0, 1.0, 1.0);
    averageSamples = false;
    explicitCubeImageBox = false;

    colorMapName = "sls";
    dataScale = std::make_shared<LinearDataScale> ();
    dataScaleName = "linear";
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
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    if(!parseCommandLine(argc, argv))
        return false;

    // Create the window and the context
    if(!createWindowAndContext())
        return false;

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

bool Application::createWindowAndContext()
{
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if(fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("SVR",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                screenWidth, screenHeight, flags);
    if(!window)
        fatalError("Failed to create window");

    glContext = SDL_GL_CreateContext(window);
    if(!glContext)
        fatalError("Failed to create window");

    return true;
}

void Application::printHelp()
{
printf(
"SVR [options] -cube cubeFile\n"
"-cube      <filename>  The data cube to display\n"
"-sw        <int>       The screen width.\n"
"-sh        <int>       The screen height.\n"
"-fovy      <number>    The vertical field of view in degrees.\n"
"-fullscreen            Use the fullscreen mode\n"
"-colormap  <name>      The color map name\n"
"-datascale <scale>     The data scale to use\n"
"-averageSampling       Render in sample averaging mode.\n"
"-cubeMappingBox  <nx ny nz px py pz>   The virtual space box to which the\n"
"                                       volume is mapped.\n"
"-sampleColorIntensity  <r g b a>       A color to multiply the samples.\n"
"-xSlice  <start> <size>       X axis slice.\n"
"-ySlice  <start> <size>       Y axis slice.\n"
"-zSlice  <start> <size>       Z axis slice.\n"
"\n"
"Available color maps:\n"
"gray\n"
"green\n"
"blue\n"
"rainbow\n"
"sls\n"
"haze\n"
"\n"
"Available data scales:\n"
"linear\n"
"log\n"
"square\n"
"sqrt\n"
"sinh\n"
"asinh\n"
"\n"
);
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
        else if(!strcmp(argv[i], "-fullscreen"))
        {
            fullscreen = true;
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
        else if(!strcmp(argv[i], "-cubeMappingBox") && (++i) + 6 <= argc)
        {
            cubeImageBox.min = glm::vec3(atof(argv[i]), atof(argv[i+1]), atof(argv[i+2]));
            cubeImageBox.max = glm::vec3(atof(argv[i+3]), atof(argv[i+4]), atof(argv[i+5]));
            i += 5;
            explicitCubeImageBox = true;
        }
        else if(!strcmp(argv[i], "-averageSampling"))
        {
            averageSamples = true;
        }
        else if(!strcmp(argv[i], "-sampleColorIntensity") && (++i) + 4 <= argc)
        {
            sampleColorIntensity = glm::vec4(atof(argv[i]), atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3]));
            i += 3;
        }
        else if(!strcmp(argv[i], "-xSlice") && (++i) + 2 <= argc)
        {
            xSlice.start = atoi(argv[i]);
            xSlice.size = atoi(argv[i+1]);
            i += 1;
        }
        else if(!strcmp(argv[i], "-ySlice") && (++i) + 2 <= argc)
        {
            ySlice.start = atoi(argv[i]);
            ySlice.size = atoi(argv[i+1]);
            i += 1;
        }
        else if(!strcmp(argv[i], "-zSlice") && (++i) + 2 <= argc)
        {
            zSlice.start = atoi(argv[i]);
            zSlice.size = atoi(argv[i+1]);
            i += 1;
        }
        else if(!strcmp(argv[i], "-h"))
        {
            printHelp();
            exit(0);
        }
    }

    if(cubeFileName.empty())
    {
        printHelp();
        return false;
    }

    return !cubeFileName.empty();
}

void Application::setDataScale(const DataScalePtr &newDataScale)
{
    dataScale = newDataScale;
    // TODO: Perform the new mapping.
}

void Application::setDataScaleNamed(const std::string &name)
{
    auto it = dataScaleNameMap.find(name);
    if(it != dataScaleNameMap.end())
    {
        dataScaleName = name;
        setDataScale(it->second);
    }
    else
    {
        dataScaleName = "linear";
        setDataScale(std::make_shared<LinearDataScale> ());
    }
}

bool Application::initializeTextures()
{
    screenColorBuffer = renderer->createTexture2D(screenWidth, screenHeight, PixelFormat::RGBA32F);
    screenColorBuffer->allocateInDevice();

    screenFramebuffer = renderer->createFramebuffer(screenWidth, screenHeight);
    screenFramebuffer->attachTexture(FramebufferAttachment::Color, screenColorBuffer);

    volumeColorBuffer = renderer->createTexture2D(screenWidth, screenHeight, PixelFormat::RGBA32F);
    volumeColorBuffer->allocateInDevice();

    return true;
}

void Application::initializeDictionaries()
{
    colorMapNameDictionary["gray"] = ColorMap::gray();
    colorMapNameDictionary["red"] = ColorMap::red();
    colorMapNameDictionary["green"] = ColorMap::green();
    colorMapNameDictionary["blue"] = ColorMap::blue();
    colorMapNameDictionary["rainbow"] = ColorMap::rainbow();
    colorMapNameDictionary["sls"] = ColorMap::sls();
    colorMapNameDictionary["haze"] = ColorMap::haze();

    dataScaleNameMap["linear"] = std::make_shared<LinearDataScale> ();
    dataScaleNameMap["log"] = std::make_shared<LogDataScale> ();
    dataScaleNameMap["square"] = std::make_shared<SquareDataScale> ();
    dataScaleNameMap["sqrt"] = std::make_shared<SquareRootDataScale> ();
    dataScaleNameMap["sinh"] = std::make_shared<SinhDataScale> ();
    dataScaleNameMap["asinh"] = std::make_shared<ASinhDataScale> ();
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
    colorMapTexture->setWrapS(TextureWrapping::ClampToEdge);
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

    computeVolumeColorBuffer = computePlatform->createImageFromTexture2D(volumeColorBuffer);
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
    if(!xSlice.isValid())
        xSlice.setWholeSize(cubeFile->getWidth());
    else
        xSlice.clampToRange(0, cubeFile->getWidth());

    if(!ySlice.isValid())
        ySlice.setWholeSize(cubeFile->getHeight());
    else
        ySlice.clampToRange(0, cubeFile->getHeight());

    if(!zSlice.isValid())
        zSlice.setWholeSize(cubeFile->getDepth());
    else
        zSlice.clampToRange(0, cubeFile->getDepth());

    for(auto &kv: cubeFile->getHeaderProperties())
        printf("%s = %s\n", kv.first.c_str(), kv.second.c_str());

    performScaleMapping();

    // Set the color map.
    setColorMapNamed(colorMapName);

    return true;
}

bool Application::initializeUI()
{
    screenWidget = std::make_shared<ContainerWidget> ();
    screenWidget->setSize(glm::vec2(screenWidth, screenHeight));

    // Title bar
    titleBar = std::make_shared<TitleBar> ();
    titleBar->setText(cubeFileName);
    screenWidget->add(titleBar);

    // Menu bar
    menuBar = std::make_shared<MenuBar> ();
    screenWidget->add(menuBar);

    // Status bar
    statusBar = std::make_shared<StatusBar> ();
    cameraPositionDisplay = statusBar->addEntry("", 2);
    scaleNameDisplay = statusBar->addEntry(dataScaleName, 1);
    screenWidget->add(statusBar);

    // Viewport widget
    viewportWidget = std::make_shared<TextureWidget> ();
    viewportWidget->setTexture(volumeColorBuffer);
    screenWidget->add(viewportWidget);

    // Color bar widget
    colorBarWidget = std::make_shared<ColorBarWidget> ();
    colorBarWidget->setGradient(colorMapTexture);
    screenWidget->add(colorBarWidget);

    // Use the data scale.
    colorBarWidget->setValueMap([this](double x) {
        return dataScale->unmapValue(x);
    });

    // Layout
    auto layout = std::make_shared<DockingLayout> ();
    layout->topElement(menuBar, 0.05);
    layout->topElement(titleBar, 0.05);
    layout->bottomElement(statusBar, 0.05);
    layout->centerElement(viewportWidget);
    layout->rightElement(colorBarWidget, 0.2);

    screenWidget->setLayout(layout);
    screenWidget->setAutoLayout(true);
    screenWidget->applyLayout();

    return true;
}

void Application::performScaleMapping()
{
    // Allocate space for the mapped fits
    size_t wholeSize = xSlice.size*ySlice.size*zSlice.size;
    std::unique_ptr<uint8_t[]> wholeData(new uint8_t[wholeSize]);

    // Map the cube.
    dataScale->mapFitsIntoU8(cubeFile, wholeData.get(), xSlice, ySlice, zSlice);

    // Create the compute buffer.
    if(!computeCubeBuffer)
    {
        computeCubeBuffer = computePlatform->createImage3D(PixelFormat::L8,
            xSlice.size, ySlice.size, zSlice.size,
            xSlice.size,
            xSlice.size*ySlice.size, (char*)wholeData.get());
    }

    // TODO: Upload the new version of the data
}

void Application::shutdown()
{
    computeCubeBuffer->destroy();
    computeVolumeColorBuffer->destroy();
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
        case SDL_MOUSEBUTTONDOWN:
            onMouseButtonDown(event.button);
            break;
        case SDL_MOUSEBUTTONUP:
            onMouseButtonUp(event.button);
            break;
        case SDL_KEYDOWN:
            onKeyDown(event.key);
            break;
        case SDL_KEYUP:
            onKeyUp(event.key);
            break;
        case SDL_QUIT:
            quit();
            break;
        case SDL_WINDOWEVENT:
            onWindowEvent(event.window);
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

    char buffer[256];
    auto newPosition = camera->getPosition();
    sprintf(buffer, "x: %f y: %f z: %f", newPosition.x, newPosition.y, newPosition.z);
    cameraPositionDisplay->setText(buffer);
}

void Application::computeCubeImageBox()
{
    if(explicitCubeImageBox)
        return;

    auto cubeExtent = glm::vec3(xSlice.size, ySlice.size, zSlice.size);
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
    auto w = xSlice.size;
    auto h = ySlice.size;
    auto d = zSlice.size;
    maxNumberOfSamples = ceil(sqrt(w*w + h*h + d*d) * lengthSamplingFactor);

    // Acquire shared resources
    renderer->beginCompute();
    computePlatform->beginCompute();
    computeVolumeColorBuffer->acquireFromRenderer(device);
    computeColorMap->acquireFromRenderer(device);

    // Setup the kernel
    auto kernel = raycastProgram->createKernel("raycastVolume");

    kernel->setBufferArg(0, computeCubeBuffer);
    kernel->setBufferArg(1, computeVolumeColorBuffer);

    // Pass the camera
    for(int i = 0; i < 8; ++i)
        kernel->setFloat4Arg(2 + i, transformedFrustum[i]);

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
    kernel->setFloatArg(20, 1.0 / colorMapTexture->getWidth());
    kernel->setFloatArg(21, colorBarWidget->getMinValue());
    kernel->setFloatArg(22, colorBarWidget->getMaxValue());

    // Color correction
    kernel->setFloatArg(23, 1.0);

    // Extra modes
    kernel->setIntArg(24, averageSamples);
    kernel->setFloat4Arg(25, sampleColorIntensity);

    // Run the rendering kernel
    //printf("Render frame %d %d\n", minNumberOfSamples, maxNumberOfSamples);
    device->runGlobalKernel2D(kernel, volumeColorBuffer->getWidth(), volumeColorBuffer->getHeight());

    // Release the shared resources.
    computeVolumeColorBuffer->releaseFromRenderer(device);
    computePlatform->endCompute();
    renderer->endCompute();
}

void Application::render3D()
{
    // Update the screen size.
    auto extent = viewportWidget->getSize();
    float aspect = extent.x/extent.y;
    camera->perspective(fovy, aspect, 0.01, 100.0);

    // Update the volume color buffer
    int width = ceil(extent.x);
    int height = ceil(extent.y);
    bool recreate = false;
    if(volumeColorBuffer->getWidth() != width || volumeColorBuffer->getHeight() != height)
    {
        computeVolumeColorBuffer->destroy();
        recreate = true;
    }

    volumeColorBuffer->resize(width, height);

    // Recreate the compute color buffer
    if(recreate)
    {
        computeVolumeColorBuffer = computePlatform->createImageFromTexture2D(volumeColorBuffer);
    }

    // Perform the rendering
    raycast();
}

void Application::render2D()
{
    auto extent = glm::vec2(screenWidth, screenHeight);

    // Resize the screen color buffer.
    screenColorBuffer->resize(screenWidth, screenHeight);

    // Draw to the screen.
    screenFramebuffer->activate();
    renderer->setScreenSize(extent);

    // Clear the window.
    renderer->clearColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
    renderer->clear();

    // Draw the screen.
    screenWidget->draw(renderer);

    renderer->flushCommands();

    // Color correct the result.
    renderer->useMainFramebuffer();

    renderer->setTexture(screenColorBuffer);
    renderer->setGammaCorrection(gammaCorrection);
    renderer->drawRectangle(glm::vec2(0.0, 0.0), extent);

    renderer->flushCommands();
}

void Application::render()
{
    if(SDL_GL_MakeCurrent(window, glContext))
        return;

    render3D();
    render2D();

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
    case SDLK_r:
        colorBarWidget->setMinValue(0.0f);
        colorBarWidget->setMaxValue(1.0f);
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

void Application::onMouseMove(const SDL_MouseMotionEvent &event)
{
    MouseMoveEvent mouseEvent;
    mouseEvent.position = glm::vec2(event.x, screenHeight - event.y);

    screenWidget->processEvent(&mouseEvent);
}

void Application::onMouseButtonDown(const SDL_MouseButtonEvent &event)
{
    MouseButtonDownEvent mouseEvent;
    mouseEvent.button = (MouseButton)event.button;
    mouseEvent.position = glm::vec2(event.x, screenHeight - event.y);

    screenWidget->processEvent(&mouseEvent);
}

void Application::onMouseButtonUp(const SDL_MouseButtonEvent &event)
{
    MouseButtonUpEvent mouseEvent;
    mouseEvent.button = (MouseButton)event.button;
    mouseEvent.position = glm::vec2(event.x, screenHeight - event.y);

    screenWidget->processEvent(&mouseEvent);
}

void Application::onWindowEvent(const SDL_WindowEvent &event)
{
    switch(event.event)
    {
    case SDL_WINDOWEVENT_RESIZED:
        screenWidget->setSize(glm::vec2(event.data1, event.data2));
        screenWidth = event.data1;
        screenHeight = event.data2;
        break;
    }
}

} // namespace SVR

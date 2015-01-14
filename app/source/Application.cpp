#include "Application.hpp"

namespace SVR
{

Application::Application()
    : isQuitting(false), window(nullptr), glContext(nullptr)
{
    screenWidth = 640;
    screenHeight = 480;
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

    if(!initializeTextures(argc, argv))
        fatalError("Failed to initialize textures");

    // Create the compute platform.
    computePlatform = createComputePlatform();
    if(!computePlatform->initialize(argc, argv))
        fatalError("Failed to initialize compute platform");

    if(!initializeComputation())
        fatalError("Failed to initialize computation");
    return true;
}

bool Application::initializeTextures(int argc, const char **argv)
{
    colorBuffer = renderer->createTexture2D(screenWidth, screenHeight, PixelFormat::RGBA32F);
    colorBuffer->allocateInDevice();

    return colorBuffer.get();
}

bool Application::initializeComputation()
{
    // Raycast program
    raycastProgram = computePlatform->loadComputeProgramFromFile("data/kernels/raycast.cl");
    raycastProgram->build();

    // Cube mapping float
    cubeMappingsFloatProgram = computePlatform->loadComputeProgramFromFile("data/kernels/cubeMappingsFloat.cl");
    cubeMappingsFloatProgram->build();

    // Cube mapping double
    cubeMappingsDoubleProgram = computePlatform->loadComputeProgramFromFile("data/kernels/cubeMappingsDouble.cl");
    cubeMappingsDoubleProgram->build();

    computeColorBuffer = computePlatform->createImageFromTexture2D(colorBuffer);

    return true;
}

void Application::shutdown()
{
    computeColorBuffer->destroy();
    raycastProgram->destroy();
    cubeMappingsFloatProgram->destroy();
    cubeMappingsDoubleProgram->destroy();

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

void Application::render()
{
    if(SDL_GL_MakeCurrent(window, glContext))
        return;

    // Update the screen size.
    auto extent = glm::vec2(screenWidth, screenHeight);
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



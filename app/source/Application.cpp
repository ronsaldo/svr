#include "Application.hpp"

namespace SVR
{

Application::Application()
    : isQuitting(false), window(nullptr), glContext(nullptr)
{
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
                640, 480,  SDL_WINDOW_OPENGL);
    if(!window)
        fatalError("Failed to create window");

    glContext = SDL_GL_CreateContext(window);
    if(!glContext)
        fatalError("Failed to create window");

    renderer = createRenderer();
    if(!renderer->initialize(argc, argv))
        fatalError("Failed to initialize the renderer");

    return true;
}

void Application::shutdown()
{
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

    // Clear the window.
    renderer->clearColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
    renderer->clear();

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



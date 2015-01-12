#ifndef _SVR_APPLICATION_HPP_
#define _SVR_APPLICATION_HPP_

#include <SDL.h>
#include <SDL_main.h>
#include "SVR/Logging.hpp"
#include "SVR/Renderer.hpp"

namespace SVR
{

/**
 * The scalable volumetric renderer application.
 */
class Application
{
public:
    Application();
    ~Application();

    int main(int argc, const char **argv);
    void quit();

private:
    bool initialize(int argc, const char **argv);
    void mainLoop();
    void shutdown();

    void processEvents();
    void render();

    void onKeyDown(const SDL_KeyboardEvent &event);
    void onKeyUp(const SDL_KeyboardEvent &event);

    bool isQuitting;
    SDL_Window *window;
    SDL_GLContext glContext;
    RendererPtr renderer;    
};

}

#endif // _SVR_APPLICATION_HPP_


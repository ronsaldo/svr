#ifndef _SVR_DICOM_APPLICATION_HPP_
#define _SVR_DICOM_APPLICATION_HPP_

#include <vector>
#include <string>
#include "SDL.h"
#include "SDL_main.h"
#include "SVR/Renderer.hpp"

namespace SVR
{
namespace Dicom
{

/**
 * SVR Dicom viewer application
 */
class DicomViewer
{
public:
    DicomViewer();
    ~DicomViewer();

    int main(int argc, const char **argv);

private:
    bool initialize(int argc, const char **argv);
    bool parseCommandLine(int argc, const char **argv);
    void printHelp();
    bool createWindow();

    void mainLoop();
    void shutdown();

    void onKeyDown(const SDL_KeyboardEvent &event);
    void onKeyUp(const SDL_KeyboardEvent &event);

    void processEvents();

    void render();
    void render2D();

    SDL_Window *window;
    SDL_GLContext glContext;
    RendererPtr renderer;

    int screenWidth, screenHeight;
    std::vector<std::string> imageFileNames;
    int currentImageIndex;
    bool imageChanged;
    int imageDepth;

    void loadImage();
    bool quitting;

    Texture2DPtr currentImageTexture;
};

} // namespace Dicom
} // namespace SVR

#endif //_SVR_DICOM_APPLICATION_HPP_

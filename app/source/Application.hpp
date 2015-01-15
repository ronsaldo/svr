#ifndef _SVR_APPLICATION_HPP_
#define _SVR_APPLICATION_HPP_

#include <SDL.h>
#include <SDL_main.h>
#include "SVR/Camera.hpp"
#include "SVR/Logging.hpp"
#include "SVR/Renderer.hpp"
#include "SVR/ComputePlatform.hpp"
#include "SVR/FitsFile.hpp"
#include "SVR/AstronomyMappings.hpp"
#include "ColorMap.hpp"

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
    bool initializeScene();
    bool initializeTextures();
    bool initializeComputation();
    bool parseCommandLine(int argc, const char **argv);

    void mainLoop();
    void shutdown();

    void processEvents();
    void render();
    void raycast();

    void setColorMap(const ColorMap &colorMap);

    void onKeyDown(const SDL_KeyboardEvent &event);
    void onKeyUp(const SDL_KeyboardEvent &event);

    bool isQuitting;
    SDL_Window *window;
    SDL_GLContext glContext;
    RendererPtr renderer;

    Texture2DPtr colorBuffer;
    Texture1DPtr colorMapTexture;
    ColorMap colorMap;

    int screenWidth, screenHeight;
    float fovy;
    float gammaCorrection;

    // Raycasting parameters
    AABox cubeImageBox;
    AABox cubeViewRegion;
    float lengthScale;

    int minNumberOfSamples;
    int maxNumberOfSamples;
    float lengthSamplingFactor;

    float filterMinValue, filterMaxValue;
    ComputeSamplerPtr currentSampler;

    // Compute platform programs and buffers.
    ComputePlatformPtr computePlatform;

    ComputeProgramPtr raycastProgram;
    ComputeProgramPtr cubeMappingsFloatProgram;
    ComputeProgramPtr cubeMappingsDoubleProgram;
    ComputeProgramPtr testProgram;

    ComputeBufferPtr computeColorMap;
    ComputeBufferPtr computeColorBuffer;
    ComputeBufferPtr computeCubeBuffer;

    CameraPtr camera;

    // Input data
    std::string cubeFileName;
    FitsFile *cubeFile;

};

}

#endif // _SVR_APPLICATION_HPP_


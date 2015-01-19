#ifndef _SVR_APPLICATION_HPP_
#define _SVR_APPLICATION_HPP_

#include <SDL.h>
#include <SDL_main.h>
#include "SVR/Camera.hpp"
#include "SVR/Logging.hpp"
#include "SVR/Renderer.hpp"
#include "SVR/ComputePlatform.hpp"
#include "SVR/FitsFile.hpp"
#include "SVR/AABox.hpp"
#include "SVR/AstronomyMappings.hpp"

#include "SVR/ColorBarWidget.hpp"

#include "ColorMap.hpp"

namespace SVR
{
/**
 * Cube mapping
 */
enum DataScale
{
    Linear = 0,
    Log
};

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

    void setColorMap(const ColorMapPtr &colorMap);
    void setColorMapNamed(const std::string &name);

    void setDataScale(DataScale dataScale);
    void setDataScaleNamed(const std::string &name);

private:
    void initializeDictionaries();
    bool initialize(int argc, const char **argv);
    bool initializeScene();
    bool initializeTextures();
    bool initializeComputation();
    bool parseCommandLine(int argc, const char **argv);

    bool initializeUI();

    void computeCubeImageBox();

    void mainLoop();
    void shutdown();

    void processEvents();
    void render();
    void raycast();
    void update(float delta);
    void performScaleMapping();

    void onKeyDown(const SDL_KeyboardEvent &event);
    void onKeyUp(const SDL_KeyboardEvent &event);
    void onMouseButtonDown(const SDL_MouseButtonEvent &event);
    void onMouseButtonUp(const SDL_MouseButtonEvent &event);

    bool isQuitting;
    SDL_Window *window;
    SDL_GLContext glContext;
    RendererPtr renderer;

    // Screen color buffer
    Texture2DPtr screenColorBuffer;
    FramebufferPtr screenFramebuffer;

    Texture2DPtr volumeColorBuffer;

    // Color mapping
    Texture1DPtr colorMapTexture;
    ColorMapPtr colorMap;
    std::string colorMapName;
    std::map<std::string, ColorMapPtr> colorMapNameDictionary;

    // Scene parameters
    int screenWidth, screenHeight;
    float fovy;
    float gammaCorrection;

    // Data visualization scale
    DataScale dataScale;
    std::map<std::string, DataScale> dataScaleNameMap;

    // Raycasting parameters
    AABox cubeImageBox;
    AABox cubeViewRegion;
    float lengthScale;

    // Raycasting samples
    int minNumberOfSamples;
    int maxNumberOfSamples;
    float lengthSamplingFactor;

    // Cube data filtering
    float filterMinValue, filterMaxValue;
    ComputeSamplerPtr currentSampler;
    ComputeSamplerPtr nearestSampler, linearSampler;

    // Compute platform programs and buffers.
    ComputePlatformPtr computePlatform;

    ComputeProgramPtr raycastProgram;
    ComputeProgramPtr cubeMappingsFloatProgram;
    ComputeProgramPtr cubeMappingsDoubleProgram;

    ComputeBufferPtr computeColorMap;
    ComputeBufferPtr computeVolumeColorBuffer;
    ComputeBufferPtr computeCubeBuffer;

    CameraPtr camera;

    // Input data
    std::string cubeFileName;
    FitsFile *cubeFile;

    // Movement
    glm::vec3 cameraVelocity;
    glm::vec3 cameraAngle;
    glm::vec3 cameraAngularVelocity;

    // UI
    ColorBarWidgetPtr colorBarWidget;
};

}

#endif // _SVR_APPLICATION_HPP_


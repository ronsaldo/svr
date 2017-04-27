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

#include "SVR/ContainerWidget.hpp"
#include "SVR/ColorBarWidget.hpp"
#include "SVR/TextureWidget.hpp"
#include "SVR/TitleBar.hpp"
#include "SVR/MenuBar.hpp"
#include "SVR/StatusBar.hpp"

#include "ColorMap.hpp"
#include "DataScale.hpp"

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

    void setColorMap(const ColorMapPtr &colorMap);
    void setColorMapNamed(const std::string &name);

    void setDataScale(const DataScalePtr &dataScale);
    void setDataScaleNamed(const std::string &name);

private:
    void initializeDictionaries();
    bool initialize(int argc, const char **argv);
    bool createWindowAndContext();

    bool initializeScene();
    bool initializeTextures();
    bool initializeComputation();
    bool parseCommandLine(int argc, const char **argv);
    void printHelp();

    bool initializeUI();

    void computeCubeImageBox();

    void mainLoop();
    void shutdown();

    void processEvents();
    void render();
    void render3D();
    void render2D();

    void raycast();
    void update(float delta);
    void performScaleMapping();

    void onKeyDown(const SDL_KeyboardEvent &event);
    void onKeyUp(const SDL_KeyboardEvent &event);
    void onMouseMove(const SDL_MouseMotionEvent &event);
    void onMouseButtonDown(const SDL_MouseButtonEvent &event);
    void onMouseButtonUp(const SDL_MouseButtonEvent &event);

    void onWindowEvent(const SDL_WindowEvent &event);

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
    bool fullscreen;
    float fovy;
    float gammaCorrection;

    // Data visualization scale
    DataScalePtr dataScale;
    std::string dataScaleName;
    std::map<std::string, DataScalePtr> dataScaleNameMap;

    // Raycasting parameters
    AABox cubeImageBox;
    AABox cubeViewRegion;
    float lengthScale;

    // Explicit cube map
    bool explicitCubeImageBox;

    // Raycasting samples
    int minNumberOfSamples;
    int maxNumberOfSamples;
    float lengthSamplingFactor;
    glm::vec4 sampleColorIntensity;
    bool averageSamples;

    // Cube data filtering
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

    // Slice range
    SliceRange xSlice;
    SliceRange ySlice;
    SliceRange zSlice;


    // UI
    ContainerWidgetPtr screenWidget;

    TextureWidgetPtr viewportWidget;
    ColorBarWidgetPtr colorBarWidget;
    TitleBarPtr titleBar;
    MenuBarPtr menuBar;
    StatusBarPtr statusBar;
    StatusBarEntryPtr cameraPositionDisplay;
    StatusBarEntryPtr scaleNameDisplay;
};

}

#endif // _SVR_APPLICATION_HPP_

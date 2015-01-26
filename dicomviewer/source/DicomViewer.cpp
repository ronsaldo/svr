#include <string.h>
#include <stdlib.h>
#include "SVR/Logging.hpp"
#include "DicomViewer.hpp"

#define Sint8 Dicom_Sint8
#define HAVE_CONFIG_H
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/dcmimage.h"

namespace SVR
{
namespace Dicom
{

DicomViewer::DicomViewer()
{
    screenWidth = 640;
    screenHeight = 640;
    quitting = false;
    currentImageIndex = 0;
}

DicomViewer::~DicomViewer()
{
}


void DicomViewer::printHelp()
{
    printf(
"dcmviewer <options> [input files]...\n"
"-sw\t\t\tScreen Width\n"
"-sh\t\t\tScreen Height\n"
    );
}

bool DicomViewer::parseCommandLine(int argc, const char **argv)
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
        else
        {
            imageFileNames.push_back(argv[i]);
        }

    }

    // Print the image file names.
    if(imageFileNames.empty())
    {
        printHelp();
        return false;
    }

    return true;
}

bool DicomViewer::initialize(int argc, const char **argv)
{
    if(!parseCommandLine(argc, argv))
        return false;

    if(!createWindow())
        return false;

    renderer = createRenderer();
    if(!renderer->initialize(argc, argv))
        return false;

    return true;
}

bool DicomViewer::createWindow()
{
    // Initialize SDL2
    if(SDL_Init(SDL_INIT_VIDEO))
    {
        fatalError("Failed to initialize SDL2");
        return false;
    }

    // Create the window
    window = SDL_CreateWindow("Dicom Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    if(!window)
    {
        fatalError("Failed to create the window");
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    if(!glContext)
        fatalError("Failed to create window");

    return true;
}

int DicomViewer::main(int argc, const char **argv)
{
    if(!initialize(argc, argv))
        return -1;

    mainLoop();
    shutdown();
    return 0;
}

void DicomViewer::mainLoop()
{
    while(!quitting)
    {
        processEvents();
        render();
        SDL_Delay(20);
    }
}

void DicomViewer::processEvents()
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
            quitting = true;
            break;
        }
    }
}

void DicomViewer::onKeyDown(const SDL_KeyboardEvent &event)
{
    switch(event.keysym.sym)
    {
    case SDLK_ESCAPE:
        quitting = true;
        break;
    case SDLK_LEFT:
        currentImageIndex = (currentImageIndex + imageFileNames.size() - 1) % imageFileNames.size();
        imageChanged = true;
        break;
    case SDLK_RIGHT:
        currentImageIndex = (currentImageIndex + 1) % imageFileNames.size();
        imageChanged = true;
        break;
    }
}

void DicomViewer::onKeyUp(const SDL_KeyboardEvent &event)
{
}

void DicomViewer::loadImage()
{
    if(currentImageTexture && !imageChanged)
        return;

    auto imageName = imageFileNames[currentImageIndex];
    auto image = new DicomImage(imageName.c_str());
    if(image)
    {
        size_t w = image->getWidth();
        size_t h = image->getHeight();
        imageDepth = image->getDepth();
        size_t size = w*h;
        if(image->getStatus() == EIS_Normal)
        {
            auto pixelData = reinterpret_cast<const uint16_t*> (image->getOutputData(16));
            if(pixelData)
            {
                if(!currentImageTexture)
                    currentImageTexture = renderer->createTexture2D(w, h, PixelFormat::L16);
                else
                    currentImageTexture->resize(w,h);
                currentImageTexture->upload(PixelFormat::L16, size, pixelData);
            }
        }
    }

    delete image;
    imageChanged = false;
}

void DicomViewer::render()
{
    if(SDL_GL_MakeCurrent(window, glContext))
        return;

    loadImage();
    render2D();

    SDL_GL_SwapWindow(window);
}

void DicomViewer::render2D()
{
    // Set the viewport.
    renderer->setScreenSize(glm::vec2(screenWidth, screenHeight));

    // Clear the screen.
    renderer->clearColor(glm::vec4(0,0,0,0));
    renderer->clear();

    if(currentImageTexture)
    {
        // Draw the image
        renderer->setTexture(currentImageTexture);
        renderer->drawRectangle(glm::vec2(0,0), glm::vec2(currentImageTexture->getWidth(), currentImageTexture->getHeight()));

        // Draw the current image name
        renderer->setTextColor(glm::vec4(1.0, 1.0, 0.2, 1.0));
        renderer->drawText(glm::vec2(20, screenHeight - 30), imageFileNames[currentImageIndex]);

        // Draw the dimensions
        char buffer[2048];
        sprintf(buffer, "Dimensions: %d x %d [%d]", (int)currentImageTexture->getWidth(), (int)currentImageTexture->getHeight(), imageDepth);
        renderer->drawText(glm::vec2(20, screenHeight - 50), buffer);

    }

    // Flush the commands.
    renderer->flushCommands();
}

void DicomViewer::shutdown()
{
    renderer->shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();
}

} // namespace Dicom
} // namespace SVR

int main(int argc, const char **argv)
{
    SVR::Dicom::DicomViewer app;
    return app.main(argc, argv);
}



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include "SVR/FitsFile.hpp"

#define HAVE_CONFIG_H
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/dcmimage.h"

using namespace SVR;

int width = -1;
int height = -1;
int depth;
std::vector<std::string> inputFileNames;
std::string outputFileName;

void printHelp()
{
}

std::string intToString(int v)
{
    char buffer[64];
    sprintf(buffer, "%d", v);
    return buffer;
}

void copyDicomInto(const std::string &inputFileName, size_t bufferSize, uint8_t *outputBuffer)
{
    // Clear the output, just in case.
    memset(outputBuffer, 0, bufferSize);

    auto image = new DicomImage(inputFileName.c_str());
    if(image)
    {
        if(image->getStatus() == EIS_Normal)
        {
            image->setMinMaxWindow();
            auto pixelData = reinterpret_cast<const uint8_t*> (image->getOutputData(8));
            if(pixelData)
            {
                printf("Copying data from %s\n", inputFileName.c_str());
                memcpy(outputBuffer, pixelData, bufferSize);
            }
        }
    }

    delete image;
}

int main(int argc, const char **argv)
{
    // Parse the command line.
    for(int i = 1; i < argc; ++i)
    {
        if(!strcmp(argv[i], "-w") && argv[++i])
        {
            width = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "-h") && argv[++i])
        {
            height = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "-o") && argv[++i])
        {
            outputFileName = argv[i];
        }
        else if(!strcmp(argv[i], "-help"))
        {
            printHelp();
            return 0;
        }
        else
        {
            inputFileNames.push_back(argv[i]);
        }

    }

    // Check the arguments
    if(width <= 0 || height <= 0 ||outputFileName.empty() || inputFileNames.empty())
    {
        printHelp();
        return -1;
    }

    depth = inputFileNames.size();
    printf("Making FITS file of size %d %d %d\n", width, height, depth);

    // Create the FITS file properties.
    FitsHeaderProperties properties;
    properties["BITPIX"] = "8";
    properties["NAXIS"] = "3";
    properties["NAXIS1"] = intToString(width);
    properties["NAXIS2"] = intToString(height);
    properties["NAXIS3"] = intToString(depth);

    // Create the output file
    auto slicePitch = width*height;
    auto outputDataSize = slicePitch*depth;
    auto outputFits = FitsFile::create(outputFileName.c_str(), properties, outputDataSize);

    auto outputBuffer = reinterpret_cast<uint8_t*> (outputFits->getImageData());

    // Start writing the slices
    for(auto &inputFileName : inputFileNames)
    {
        copyDicomInto(inputFileName, slicePitch, outputBuffer);
        outputBuffer += slicePitch;
    } 

    // Close the output fits.
    outputFits->close();
    delete outputFits;
    
    return 0;
}


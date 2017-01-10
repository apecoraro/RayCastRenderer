#include "RayCaster.h"
#include "SmartPtr.h"

#include "Sphere.h"
#include "Box.h"
#include "Matrix.h"
#include "Ellipsoid.h"
#include "Ray.h"
#include "Polygon.h"
#include "Image.h"
#include "Timer.h"
#include "Sampler.h"
#include "UniformGridSampler.h"
#include "JitteredGridSampler.h"

#include <iostream>
#include <sstream>
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace cs500;

static void PrintUsage(char* argv[])
{
    std::cout << argv[0] << " <path to input scene file>.txt "
                         << "<path to output image> <resolution of output image> "
                         << "<anti-aliasing horiz sample count> <anti-alising vert sample count> "
                         << "<anti-aliasing mode (0=none or 1=uniform or 2=jittered)> " 
                         << "[num soft shadow feelers - defaults to zero (i.e. hard shadows)] [num blur rays - defaults to zero (i.e. no blur)]" << std::endl;
}

static bool ParseInputArgs(int argc, char* argv[], 
                            std::string& inputFilePath, 
                            std::string& outputFilePath, 
                            size_t& outputHorizRes,
                            int& horizSampleCount,
                            int& vertSampleCount,
                            int& samplingMode,
                            int& numShadowFeelers,
                            int& numBlurRays)
{
    std::stringstream inputArgs;
    for(int i = 1; i < argc; ++i)
    {
        inputArgs << argv[i] << std::endl;
    }

    inputArgs >> inputFilePath;
    inputArgs >> outputFilePath;
    inputArgs >> outputHorizRes;
    inputArgs >> horizSampleCount;
    inputArgs >> vertSampleCount;
    inputArgs >> samplingMode;

    if(argc > 7)
    {
        inputArgs >> numShadowFeelers;
        if(argc > 8)
            inputArgs >> numBlurRays;
    }

    if(inputArgs.fail())
    {
        std::cerr << "Error: failed to parse input args." << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    if(argc < 6)
    {
        PrintUsage(argv);
        return 1;
    }

    std::string inputFilePath;
    std::string outputFilePath;
    size_t outputHorizRes;
    int vertSampleCount;
    int horizSampleCount;
    int sampleMode;
    bool renderTiles = false;
    bool useSphereTree = false;
    int shadowFeelers = 0;
    int blurRays = 0;
    if(!ParseInputArgs(argc, argv, 
                       inputFilePath, 
                       outputFilePath, 
                       outputHorizRes,
                       horizSampleCount,
                       vertSampleCount,
                       sampleMode,
                       shadowFeelers,
                       blurRays))
    {
        PrintUsage(argv);
        return 1;
    }

    std::cout << "Running with args: " 
              << "input file: " << inputFilePath 
              << std::endl
              << " output file: " 
              << outputFilePath 
              << std::endl
              << " horiz resolution: " 
              << outputHorizRes 
              << std::endl
              << " anti-aliasing horiz sample count: "
              << horizSampleCount
              << std::endl
              << " anti-aliasing vert sample count: "
              << vertSampleCount
              << std::endl
              << " sampling mode: "
              << sampleMode
              << std::endl
              << " num soft shadow feelers: "
              << shadowFeelers
              << std::endl
              << " num blur rays: "
              << blurRays 
              << std::endl;

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
   
    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    RayCaster rayCaster;

    double elapsed = 0;
    if(rayCaster.loadScene(inputFilePath))
    {
        Sampler defaultSampler;
        UniformGridSampler uniformSampler(horizSampleCount, vertSampleCount);
        JitteredGridSampler jitteredSampler(horizSampleCount, vertSampleCount);
        Sampler* pSampler = &defaultSampler;
        if(sampleMode == 1)
            pSampler = &uniformSampler;
        else if(sampleMode == 2)
            pSampler = &jitteredSampler;

        if(renderTiles)
            pSampler->setRenderTiles(true);
        if(useSphereTree)
            rayCaster.buildSphereTree();
            //rayCaster.buildKDTree();
        if(shadowFeelers > 1)
        {
            rayCaster.setDrawSoftShadows(true, shadowFeelers);
        }

        if(blurRays > 1)
        {
            rayCaster.setDrawBlurredReflections(true, blurRays, 0.5f);
            rayCaster.setDrawBlurredTransmissions(true, blurRays, 0.05f);
        }

        const Camera* pCamera = rayCaster.getCamera();
        if(pCamera != NULL)
        {
            Timer_t start = Timer::instance()->tick();
            SmartPtr<cs500::Image> spImage = pSampler->generateImage(*pCamera,
                                                                     rayCaster, 
                                                                     (int)outputHorizRes);
            
            elapsed = Timer::instance()->getElapsedSecs(start, Timer::instance()->tick());
            if(spImage.get() != NULL)
            {
                spImage->saveBitmap(outputFilePath);
                spImage = NULL;
            }
        }
    }
    
    GdiplusShutdown(gdiplusToken);

    std::cout << "Elapsed seconds: " << elapsed << std::endl;

    return 0;
} 
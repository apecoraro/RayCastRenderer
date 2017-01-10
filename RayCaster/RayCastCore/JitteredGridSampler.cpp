#include "JitteredGridSampler.h"

#include <stdlib.h>
#include <time.h>

using namespace cs500;

JitteredGridSampler::JitteredGridSampler(int horizSampleCount, int vertSampleCount) :
    UniformGridSampler(horizSampleCount, vertSampleCount)
{
    srand((unsigned int)time(NULL));
}

void JitteredGridSampler::computePixelColor(int row, 
                                            int col, 
                                            int maxDepth, 
                                            const RayCaster& rayCaster, 
                                            Vec3& color) const
{
    for(int subPixelRow = 0; subPixelRow < numVertSamples(); ++subPixelRow)
    {
        float beta = 1.0f;

        for(int subPixelCol = 0; subPixelCol < numHorizSamples(); ++subPixelCol)
        {
            float jitteredOffsetV = (0.0001f + ((float)(rand() % 100) / 100.0f));

            Vec3 betaV = camera().vAxis();
            betaV *= ((beta - (row * pixelHeight())) + 
                     (halfPixelHeight() - (subPixelRow * subPixelHeight()) 
                     - (subPixelHeight() * jitteredOffsetV)));

            float alpha = -1.0f;

            float jitteredOffsetU = (0.0001f + ((float)(rand() % 100) / 100.0f));

            Vec3 alphaU = camera().uAxis();
            alphaU *= ((alpha + (col * pixelWidth())) + 
                      (-halfPixelWidth() + (subPixelCol * subPixelWidth()) 
                      + (subPixelWidth() * jitteredOffsetU)));

            Vec3 viewPlanePoint = camera().center();
            viewPlanePoint += betaV;
            viewPlanePoint += alphaU;

            Ray ray(eyePos(), Vec3::normalize(viewPlanePoint - eyePos()));

            bool inAir = true;
            Vec3 subPixelColor;
            rayCaster.traceRay(ray, subPixelColor, maxDepth);

            color += subPixelColor;
        }
    }

    color /= static_cast<float>((numVertSamples() * numHorizSamples()));
}
    
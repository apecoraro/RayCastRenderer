#include "UniformGridSampler.h"
#include "RayCaster.h"

using namespace cs500;

UniformGridSampler::UniformGridSampler(int horizSampleCount, int vertSampleCount) :
    _numHorizSamples(horizSampleCount),
    _numVertSamples(vertSampleCount)
{
}

void UniformGridSampler::initialize(const Camera& cam, int horizRes)
{
    Sampler::initialize(cam, horizRes);

    _subPixelHeight = pixelHeight() / _numVertSamples;
    _subPixelWidth = pixelWidth() / _numHorizSamples;

    _halfSubPixelHeight = _subPixelHeight * 0.5f;
    _halfSubPixelWidth = _subPixelWidth * 0.5f;

    _halfPixelHeight = pixelHeight() * 0.5f;
    _halfPixelWidth = pixelWidth() * 0.5f;
}        

void UniformGridSampler::computePixelColor(int row, 
                                           int col, 
                                           int maxDepth, 
                                           const RayCaster& rayCaster, 
                                           Vec3& color) const
{
    for(int subPixelRow = 0; subPixelRow < _numVertSamples; ++subPixelRow)
    {
        float beta = 1.0f;

        Vec3 betaV = camera().vAxis();
        betaV *= ((beta - (row * pixelHeight())) + (_halfPixelHeight - (subPixelRow * _subPixelHeight) - _halfSubPixelHeight));

        for(int subPixelCol = 0; subPixelCol < _numHorizSamples; ++subPixelCol)
        {
            float alpha = -1.0f;

            Vec3 alphaU = camera().uAxis();
            alphaU *= ((alpha + (col * pixelWidth())) + (-_halfPixelWidth + (subPixelCol * _subPixelWidth) + _halfSubPixelWidth));

            Vec3 viewPlanePoint = camera().center();
            viewPlanePoint += betaV;
            viewPlanePoint += alphaU;

            Ray ray(eyePos(), Vec3::normalize(viewPlanePoint - eyePos()));

            Vec3 subPixelColor;
            rayCaster.traceRay(ray, subPixelColor, maxDepth);

            color += subPixelColor;
        }
    }

    color /= static_cast<float>((_numVertSamples * _numHorizSamples));
}
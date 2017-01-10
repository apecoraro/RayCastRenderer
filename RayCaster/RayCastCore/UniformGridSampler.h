#pragma once

#include "Sampler.h"

namespace cs500
{
    class UniformGridSampler : public Sampler
    {
    public:
        UniformGridSampler(int horizSampleCount, int vertSampleCount);

        virtual void initialize(const Camera& cam, int horizRes);

        virtual void computePixelColor(int row, 
                                   int col, 
                                   int maxDepth, 
                                   const RayCaster& rayCaster, 
                                   Vec3& color) const;

        int numHorizSamples() const { return _numHorizSamples; }
        int numVertSamples() const { return _numVertSamples; }
        float subPixelHeight() const { return _subPixelHeight; }
        float subPixelWidth() const { return _subPixelWidth; }
        float halfSubPixelHeight() const { return _halfSubPixelHeight; }
        float halfSubPixelWidth() const { return _halfSubPixelWidth; }
        float halfPixelWidth() const { return _halfPixelWidth; }
        float halfPixelHeight() const { return _halfPixelHeight; }

    private:
        int _numHorizSamples;
        int _numVertSamples;
        float _subPixelHeight;
        float _subPixelWidth;
        float _halfSubPixelHeight;
        float _halfSubPixelWidth;
        float _halfPixelWidth;
        float _halfPixelHeight;
    };
};
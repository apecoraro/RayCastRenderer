#pragma once

#include "UniformGridSampler.h"

namespace cs500
{
    class JitteredGridSampler : public UniformGridSampler
    {
    public:
        JitteredGridSampler(int horizSampleCount, int vertSampleCount);

        virtual void computePixelColor(int row, 
                                   int col, 
                                   int maxDepth, 
                                   const RayCaster& rayCaster, 
                                   Vec3& color) const;
    };
};
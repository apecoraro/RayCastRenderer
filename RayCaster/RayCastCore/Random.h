#pragma once

namespace cs500
{
    class Random
    {
    public:
        static void Initialize(unsigned int seed);

        static float Get(float minVal, float maxVal);
    };
};
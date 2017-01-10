#include "Random.h"

#include <stdlib.h>

using namespace cs500;

void Random::Initialize(unsigned int seed)
{
    srand(seed);
}

float Random::Get(float minValue, float maxValue)
{
    int randomInt = rand();

    float pctMax = (float)randomInt / (float)RAND_MAX;

    float diff = maxValue - minValue;

    return minValue + (diff * pctMax);
}
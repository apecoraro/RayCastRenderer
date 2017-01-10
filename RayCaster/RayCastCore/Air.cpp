#include "Air.h"

#include <math.h>

using namespace cs500;

Air::Air() :
    _relativeElecPerm(1.0f),
    _relativeMagPerm(1.0f),
    _indexOfRefraction(1.0f),
    _attenuationFactors(1.0f, 1.0f, 1.0f)
{
}
    
Air::Air(float relElecPerm, float relMagPerm, const Vec3& attenuation) :
    _relativeElecPerm(relElecPerm),
    _relativeMagPerm(relMagPerm),
    _indexOfRefraction(sqrtf(relElecPerm * relMagPerm)),
    _attenuationFactors(attenuation)
{
}

void Air::setRelativeElecPerm(float rep)
{
    _relativeElecPerm = rep;
    _indexOfRefraction = sqrtf(_relativeElecPerm * _relativeMagPerm);
}

void Air::setRelativeMagPerm(float rmp)
{
    _relativeMagPerm = rmp;
    _indexOfRefraction = sqrtf(_relativeElecPerm * _relativeMagPerm);
}
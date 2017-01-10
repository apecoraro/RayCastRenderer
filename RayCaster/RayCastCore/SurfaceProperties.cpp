#include "SurfaceProperties.h"

#include <math.h>

using namespace cs500;

SurfaceProperties::SurfaceProperties() :
    _specularCoef(0.0f),
    _specularExp(0.0f),
    _relativeElecPerm(0.0f),
    _relativeMagPerm(0.0f),
    _indexOfRefraction(0.0f)
{
}

SurfaceProperties::SurfaceProperties(const Vec3& diffuse,
                                     float specularCoef,
                                     float specularExp,
                                     const Vec3& attenuationFactors,
                                     float relativeElecPerm,
                                     float relativeMagPerm) :
    _diffuse(diffuse),
    _specularCoef(specularCoef),
    _specularExp(specularExp),
    _attenuationFactors(attenuationFactors),
    _relativeElecPerm(relativeElecPerm),
    _relativeMagPerm(relativeMagPerm),
    _indexOfRefraction(sqrtf(relativeElecPerm * relativeMagPerm))
{
}

void SurfaceProperties::setRelativeElecPerm(float rep)
{
    _relativeElecPerm = rep;
    _indexOfRefraction = sqrtf(_relativeElecPerm * _relativeMagPerm);
}

void SurfaceProperties::setRelativeMagPerm(float rmp)
{
    _relativeMagPerm = rmp;
    _indexOfRefraction = sqrtf(_relativeElecPerm * _relativeMagPerm);
}

const Vec3& SurfaceProperties::textureColor(const Vec2& texUV) const
{
    if(_spTexture.get() != NULL)
    {
        return _spTexture->getColor(texUV.x(), texUV.y());
    }
    else
    {
        static Vec3 noTextureColor(1.0f, 0.0f, 1.0f);

        return noTextureColor;
    }
}
#pragma once

#include "Referenced.h"
#include "Vec3.h"

namespace cs500
{
    class Air : public Referenced
    {
    public:
        Air();
        Air(float relElecPerm, float relMagPerm, const Vec3& attenuation);

        const Vec3& attenuationFactors() const { return _attenuationFactors; }
        float relativeElecPerm() const { return _relativeElecPerm; }
        float relativeMagPerm() const { return _relativeMagPerm; }
        float indexOfRefraction() const { return _indexOfRefraction; }

        void setAttenuationFactors(const Vec3& attFac) { _attenuationFactors = attFac; }
        void setRelativeElecPerm(float rep);
        void setRelativeMagPerm(float rmp);

    private:
        float _relativeElecPerm;
        float _relativeMagPerm;
        float _indexOfRefraction;
        Vec3 _attenuationFactors;
    };
};
#pragma once

#include "Referenced.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Texture.h"
#include "SmartPtr.h"

namespace cs500
{
    class SurfaceProperties : public Referenced
    {
    public:
        SurfaceProperties();
        SurfaceProperties(const Vec3& diffuse,
                          float specularCoef,
                          float specularExp,
                          const Vec3& attenuationFactors,
                          float relativeElecPerm,
                          float relativeMagPerm);

        const Vec3& diffuse() const { return _diffuse; }
        const Vec3& textureColor(const Vec2& texUV) const;
        float specularCoef() const { return _specularCoef; }
        float specularExp() const { return _specularExp; }
        const Vec3& attenuationFactors() const { return _attenuationFactors; }
        float relativeElecPerm() const { return _relativeElecPerm; }
        float relativeMagPerm() const { return _relativeMagPerm; }
        float indexOfRefraction() const { return _indexOfRefraction; }

        void setDiffuse(const Vec3& diffuse) { _diffuse = diffuse; }
        void setSpecularCoef(float specCoef) { _specularCoef = specCoef; }
        void setSpecularExp(float specExp) { _specularExp = specExp; }
        void setAttenuationFactors(const Vec3& attFac) { _attenuationFactors = attFac; }
        void setRelativeElecPerm(float rep);
        void setRelativeMagPerm(float rmp);
        void setTexture(const Texture* pTexture) { _spTexture = pTexture; }

    protected:
        virtual ~SurfaceProperties() {}
    private:
        Vec3 _diffuse;
        float _specularCoef;
        float _specularExp;
        Vec3 _attenuationFactors;
        float _relativeElecPerm;
        float _relativeMagPerm;
        float _indexOfRefraction;
        SmartPtr<const Texture> _spTexture;
    };
};
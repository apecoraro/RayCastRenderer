#pragma once

#include "Referenced.h"
#include "SmartPtr.h"
#include "Vec3.h"
#include "Air.h"

namespace cs500
{
    class Environment
    {
    public:
        Environment();

        const Vec3& ambientLight() const { return _ambient; }
        void setAmbientLight(const Vec3& intensity) { _ambient = intensity; }

        const Air& air() const { return *_spAir.get(); }
        void setAir(Air& air) { _spAir = &air; }

    private:
        Vec3 _ambient;
        SmartPtr<Air> _spAir;
    };
};
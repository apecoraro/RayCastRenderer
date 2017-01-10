#pragma once

#include "Vec3.h"
#include "Referenced.h"

namespace cs500
{
    class Camera : public Referenced
    {
    public:
        Camera() {}
        Camera(const Vec3& center,
               const Vec3& uVec,
               const Vec3& vVec,
               const Vec3& eyeVec);

        const Vec3& center() const { return _center; }
        const Vec3& uAxis() const { return _uVec; }
        const Vec3& vAxis() const { return _vVec; }
        const Vec3& eyeVec() const { return _eyeVec; }

        const Vec3& eyePos() const { return _eyePos; }

        float pixelSize(int horizRes) const;
    protected:
        virtual ~Camera() {}

    private:
        Vec3 _center;
        Vec3 _uVec;
        Vec3 _vVec;
        Vec3 _eyeVec;
        Vec3 _eyePos;
        //uVec, vVec, and eyeVec are relative to _center
    };
};
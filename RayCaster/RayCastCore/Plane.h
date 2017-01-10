#pragma once

#include "Vec3.h"

namespace cs500
{
    class Plane
    {
    public:
        Plane() {}
        Plane(const Vec3& pointOnPlane, const Vec3& normal);

        void set(const Vec3& pointOnPlane, const Vec3& normal);

        const Vec3& pointOnPlane() const { return _pointOnPlane; }
        const Vec3& normal() const { return _normal; }
        float planeD() const { return _planeD; }
        float eval(const Vec3& p) const { return _normal.x()*p.x() + _normal.y()*p.y() + _normal.z()*p.z() + _planeD; }
    private:
        Vec3 _pointOnPlane;
        Vec3 _normal;
        //The D from the plane equation Ax + By + Cz + D = 0
        float _planeD;
    };
};
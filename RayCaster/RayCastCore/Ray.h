#pragma once

#include "Vec3.h"
#include "Vec2.h"
#include "BoundingVolumes.h"

#include <float.h>
#include <vector>

namespace cs500
{
    class Sphere;
    class Box;
    class Plane;
    class Polygon;
    class Ellipsoid;

    struct ISectData
    {
        float tMin;
        float tMax;
        Vec3 normalMin;
        Vec3 normalMax;
        ISectData() : tMin(0.0f), tMax(FLT_MAX) {}
    };

    class Ray
    {
    public:
        Ray(const Vec3& start, const Vec3& dir);

        Vec3 getPoint(float tval) const;
        float intersect(const Sphere& sphere, Vec3& isectPt, Vec3& isectNorm) const;
        float intersect(const Box& box, Vec3& isectPt, Vec3& isectNorm) const;
        float intersect(const Polygon& polygon, 
                        Vec3& isectPt, 
                        Vec3& isectNorm, 
                        bool& isCurveShaded, Vec3& shadingNorm,
                        bool& hasTexCoords, Vec2& texCoords) const;
        float intersect(const Ellipsoid& ellipsoid, Vec3& isectPt, Vec3& isectNorm) const;

        typedef std::vector<Plane> PlaneList;

        bool intersect(const PlaneList& planeList, ISectData& isectData) const;
        bool intersect(const Plane& plane, float& t) const;
        bool intersect(const BoundingSphere& sphere, float& t) const;

        const Vec3& start() const { return _start; }
        Vec3& start() { return _start; }
        const Vec3& direction() const { return _dir; }
        Vec3& direction() { return _dir; }

        void setStart(const Vec3& start) { _start = start; }
        void setDirection(const Vec3& dir) { _dir = dir; }
    private:
        Vec3 _start;
        Vec3 _dir;
    };
};
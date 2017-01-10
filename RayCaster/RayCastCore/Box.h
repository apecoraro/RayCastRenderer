#pragma once

#include "Node.h"
#include "Vec3.h"
#include "Plane.h"

namespace cs500
{
    class Box : public Node
    {
    public:
        Box();

        Box(const Vec3& corner,
            const Vec3& lenVec,
            const Vec3& widVec,
            const Vec3& hghtVec);

        void set(const Vec3& corner,
                 const Vec3& lenVec,
                 const Vec3& widVec,
                 const Vec3& hghtVec);

        const Vec3& corner() const { return _corner; }
        const Vec3& lengthVector() const { return _lengthVec; }
        const Vec3& widthVector() const { return _widthVec; }
        const Vec3& heightVector() const { return _heightVec; }
        Vec3 center() const 
        { 
            Vec3 centerVec = _lengthVec + _widthVec + _heightVec;
            centerVec *= 0.5f;
            return _corner + centerVec;
        }

        const Plane& getPlane(size_t index) const { return _planes[index]; }

        virtual bool intersect(const Ray& ray, 
                               IntersectionData& data) const;

        virtual BoundingSphere computeBoundingSphere() const;
        virtual BoundingBox computeBoundingBox() const;
    protected:
        void computePlanes();
    private:
        Vec3 _corner;
        Vec3 _lengthVec;
        Vec3 _widthVec;
        Vec3 _heightVec;
        Plane _planes[6];
    };
};
#pragma once

#include "Node.h"
#include "Vec3.h"

namespace cs500
{
    class Ray;

    class Sphere : public Node
    {
    public:
        Sphere() : _radius(-1.0f), _radiusSq(-1.0f) {}

        Sphere(const Vec3& c, float r) : _center(c), _radius(r), _radiusSq(r * r) {}

        virtual bool intersect(const Ray& ray, 
                               IntersectionData& data) const;

        const Vec3& center() const { return _center; }
        void setCenter(const Vec3& center) { _center = center; _boundsNeedUpdating=true; }
        float radius() const { return _radius; }
        void setRadius(float r) { _radius = r; _radiusSq = r*r; _boundsNeedUpdating=true; }
        float radiusSq() const { return _radiusSq; }

        virtual BoundingSphere computeBoundingSphere() const
        {
            return BoundingSphere(_center, _radius);
        }
        virtual BoundingBox computeBoundingBox() const;
    private:
        Vec3 _center;
        float _radius;
        float _radiusSq;
    };
}
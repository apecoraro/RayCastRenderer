#include "Sphere.h"

using namespace cs500;

bool Sphere::intersect(const Ray& ray, 
                       IntersectionData& data) const
{
    float isectTVal = ray.intersect(*this, data.isectPt, data.isectNorm);
    if(isectTVal >= 0.0f)
    {
        data.isectTVal = isectTVal;
        data.spIntersectedObject = this;
        return true;
    }

    return false;
}

BoundingBox Sphere::computeBoundingBox() const
{
    BoundingBox bbox;

    Vec3 minPt(_center.x()-_radius, _center.y()-_radius, _center.z()-_radius);

    bbox.expandBy(minPt);

    Vec3 maxPt(_center.x()+_radius, _center.y()+_radius, _center.z()+_radius);

    bbox.expandBy(maxPt);

    return bbox;
}
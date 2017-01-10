#include "Plane.h"

using namespace cs500;

//calculates the D in the Ax + By + Cz + D = 0 plane equation
static float CalcPlaneD(const Vec3& pointOnPlane, const Vec3& planeNormal)
{
    float Ax = (planeNormal.x() * pointOnPlane.x());
    float By = (planeNormal.y() * pointOnPlane.y());
    float Cz = (planeNormal.z() * pointOnPlane.z());
    return (-Ax - By - Cz);
}

Plane::Plane(const Vec3& pointOnPlane, const Vec3& normal) :
    _pointOnPlane(pointOnPlane),
    _normal(normal),
    _planeD(CalcPlaneD(_pointOnPlane, _normal))
{

}

void Plane::set(const Vec3& pointOnPlane, const Vec3& normal)
{
    _pointOnPlane = pointOnPlane;
    _normal = normal;
    _planeD = CalcPlaneD(_pointOnPlane, _normal);
}
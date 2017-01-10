#include "Ellipsoid.h"

using namespace cs500;

bool Ellipsoid::intersect(const Ray& ray, 
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

void Ellipsoid::computeIntersectionMatrices()
{
    _unitSphereToEllipsoidMatrix.set(_uAxis.x(), _vAxis.x(), _wAxis.x(), _center.x(),
                                     _uAxis.y(), _vAxis.y(), _wAxis.y(), _center.y(),
                                     _uAxis.z(), _vAxis.z(), _wAxis.z(), _center.z(),
                                           0.0f,       0.0f,       0.0f,       1.0f);

    _ellipsoidToUnitSphereMatrix.invert(_unitSphereToEllipsoidMatrix);

    _unitSphereNormToEllipsoidNormMatrix = _ellipsoidToUnitSphereMatrix;

    _unitSphereNormToEllipsoidNormMatrix.transpose();
}

BoundingSphere Ellipsoid::computeBoundingSphere() const
{
    float uLenSq = _uAxis.lengthSq();
    float vLenSq = _vAxis.lengthSq();
    float wLenSq = _wAxis.lengthSq();
    if(uLenSq > vLenSq)
    {
        if(uLenSq > wLenSq)
            return BoundingSphere(_center, sqrtf(uLenSq));
    }
    else
    {
        if(vLenSq > wLenSq)
            return BoundingSphere(_center, sqrtf(vLenSq));
    }
        
    return BoundingSphere(_center, sqrtf(wLenSq));        
}

BoundingBox Ellipsoid::computeBoundingBox() const
{
    BoundingBox bbox;
    
    Vec3 uMax = _center + _uAxis;
    bbox.expandBy(uMax);

    Vec3 uMin = _center - _uAxis;
    bbox.expandBy(uMin);

    Vec3 vMax = _center + _vAxis;
    bbox.expandBy(vMax);
    Vec3 vMin = _center - _vAxis;
    bbox.expandBy(vMin);

    Vec3 wMax = _center + _wAxis;
    bbox.expandBy(wMax);
    Vec3 wMin = _center - _wAxis;
    bbox.expandBy(wMin);

    return bbox;
}
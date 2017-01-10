#include "Box.h"

using namespace cs500;

Box::Box()
{
}
 
Box::Box(const Vec3& corner,
         const Vec3& lenVec,
         const Vec3& widVec,
         const Vec3& hghtVec) : 
    _corner(corner),
    _lengthVec(lenVec),
    _widthVec(widVec),
    _heightVec(hghtVec)
{
    computePlanes();
}

void Box::set(const Vec3& corner,
              const Vec3& lenVec,
              const Vec3& widVec,
              const Vec3& hghtVec)
{
    _corner = corner;
    _lengthVec = lenVec;
    _widthVec = widVec;
    _heightVec = hghtVec;
    computePlanes();
    _boundsNeedUpdating = true;
}

bool Box::intersect(const Ray& ray, 
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

void Box::computePlanes()
{
    //Front
    _planes[0].set(_corner, Vec3::cross(_lengthVec, _heightVec));
    //Back
    _planes[1].set(_corner + _widthVec, -_planes[0].normal());
    //Left
    _planes[2].set(_corner, Vec3::cross(_heightVec, _widthVec));
    //Right
    _planes[3].set(_corner + _lengthVec,  -_planes[2].normal());
    //Bottom
    _planes[4].set(_corner, Vec3::cross(_widthVec, _lengthVec));
    //Top
    _planes[5].set(_corner + _heightVec, -_planes[4].normal());
}

BoundingSphere Box::computeBoundingSphere() const
{
    Vec3 vecToCenter((_lengthVec * 0.5f) + (_heightVec * 0.5f) + (_widthVec * 0.5f));
    Vec3 center = _corner + vecToCenter;
    return BoundingSphere(center, vecToCenter.length());
}

BoundingBox Box::computeBoundingBox() const
{
    BoundingBox bbox;

    bbox.expandBy(_corner);

    bbox.expandBy(_corner + _heightVec);

    bbox.expandBy(_corner + _widthVec);

    bbox.expandBy(_corner + _lengthVec);

    return bbox;
}
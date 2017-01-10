#include "BoundingVolumes.h"

using namespace cs500;

BoundingSphere::BoundingSphere() :
    _center(0.0f, 0.0f, 0.0f),
    _radius(-1.0f),
    _radiusSq(-1.0f)
{
}
        
BoundingSphere::BoundingSphere(const Vec3& center,
                               float radius) :
    _center(center),
    _radius(radius),
    _radiusSq(radius * radius)
{
}

BoundingSphere::~BoundingSphere()
{
}

const Vec3& BoundingSphere::center() const
{
    return _center;
}

float BoundingSphere::radius() const
{
    return _radius;
}

float BoundingSphere::radiusSq() const
{
    return _radiusSq;
}

static void ExpandSphereBySphere(BoundingSphere& sphere, const Vec3& newSphereCenter, float newSphereRadius)
{
    if(newSphereRadius <= 0.0f) 
        return;

    if(sphere.radius() <= 0.0f)
    {
        sphere.setCenter(newSphereCenter);
        sphere.setRadius(newSphereRadius);
        return;
    }
    
    float distBetween = (sphere.center() - newSphereCenter).length();

    // New sphere is contained inside this one
    if(distBetween + newSphereRadius <= sphere.radius())  
    {
        return;
    }

    //  New sphere completely contains this one 
    if(distBetween + sphere.radius() <= newSphereRadius )  
    {
        sphere.setCenter(newSphereCenter);
        sphere.setRadius(newSphereRadius);
        return;
    }
    
    //Build a new sphere that completely contains the other two
    //Use similar triangles
    float expandedRadius = (sphere.radius() + distBetween + newSphereRadius) * 0.5f;
    float ratio = (expandedRadius - sphere.radius()) / distBetween;

    Vec3 expandedSphereCenter(sphere.center().x() + (newSphereCenter.x() - sphere.center().x()) * ratio,
                              sphere.center().y() + (newSphereCenter.y() - sphere.center().y()) * ratio,
                              sphere.center().z() + (newSphereCenter.z() - sphere.center().z()) * ratio);

    sphere.setCenter(expandedSphereCenter);
    sphere.setRadius(expandedRadius);
}

void BoundingSphere::expandBy(const BoundingSphere& newSphere)
{
    ExpandSphereBySphere(*this, newSphere.center(), newSphere.radius());
}

void BoundingSphere::expandBy(const Vec3& v)
{
    if (valid())
    {
        Vec3 dv = v - _center;
        float r = dv.length();
        if(r > _radius)
        {
            float dr = (r - _radius) * 0.5f;
            _center += dv * (dr / r);
            _radius += dr;
            _radiusSq = _radius * _radius;
        }
    }
    else
    {
        _center = v;
        _radius = 0.0;
    }
}
       
bool BoundingSphere::contains(const Vec3& pos) const
{
    Vec3 diff = pos - _center;

    float lenSq = diff.lengthSq();

    return lenSq < _radiusSq;
}

void BoundingBox::expandBy(const BoundingBox& bbox)
{
    expandBy(bbox.getCorner());
    expandBy(bbox.getFarCorner());
}

void BoundingBox::expandBy(const Vec3& point)
{
    if(!valid())
    {
        _corner = point;
        _sizeVec = Vec3(0.0f, 0.0f, 0.0f);
        return;
    }

    Vec3 far = getFarCorner();

    if(point.x() < _corner.x())
        _corner.x() = point.x();

    if(point.y() < _corner.y())
        _corner.y() = point.y();

    if(point.z() < _corner.z())
        _corner.z() = point.z();

    if(point.x() > far.x())
        far.x() = point.x();

    if(point.y() > far.y())
        far.y() = point.y();

    if(point.z() > far.z())
        far.z() = point.z();

    _sizeVec = far - _corner;
}
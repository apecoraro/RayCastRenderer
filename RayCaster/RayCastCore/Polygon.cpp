#include "Polygon.h"

using namespace cs500;

void Polygon::addTriangle(const Triangle& triangle)
{
    _triangles.push_back(triangle);
    _boundsNeedUpdating = true;
}

void Polygon::addTriangle(const Vec3& p1,
                          const Vec3& p2,
                          const Vec3& p3)
{
    _triangles.push_back(Triangle(p1, p2, p3));
    _boundsNeedUpdating = true;
}

bool Polygon::intersect(const Ray& ray, 
                        IntersectionData& data) const
{
    float isectTVal = ray.intersect(*this, data.isectPt, 
                                    data.isectNorm, 
                                    data.isCurveShaded, 
                                    data.shadingNorm,
                                    data.hasTexCoords,
                                    data.texCoords);
    if(isectTVal >= 0.0f)
    {
        data.isectTVal = isectTVal;
        data.spIntersectedObject = this;
        return true;
    }

    return false;
}

BoundingSphere Polygon::computeBoundingSphere() const
{
    BoundingSphere bounds;
    for(Triangles::const_iterator itr = _triangles.begin();
        itr != _triangles.end();
        ++itr)
    {
        const Triangle& tri = *itr;
        bounds.expandBy(tri.points(0));
        bounds.expandBy(tri.points(1));
        bounds.expandBy(tri.points(2));
    }

    return bounds;
}

BoundingBox Polygon::computeBoundingBox() const
{
    BoundingBox bounds;
    for(Triangles::const_iterator itr = _triangles.begin();
        itr != _triangles.end();
        ++itr)
    {
        const Triangle& tri = *itr;
        bounds.expandBy(tri.points(0));
        bounds.expandBy(tri.points(1));
        bounds.expandBy(tri.points(2));
    }

    return bounds;
}
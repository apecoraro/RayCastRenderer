#include "Triangle.h"

//#include "Plane.h"
//#include "Ray.h"

using namespace cs500;

static Vec3 CalcNormal(const Vec3& p1, 
                       const Vec3& p2,
                       const Vec3& p3) 
{
    float ux, uy, uz, vx, vy, vz;

    ux = p2.x() - p1.x();
    uy = p2.y() - p1.y();
    uz = p2.z() - p1.z();
    vx = p3.x() - p1.x();
    vy = p3.y() - p1.y();
    vz = p3.z() - p1.z();

    return Vec3(uy*vz - uz*vy,
                uz*vx - ux*vz,
                ux*vy - uy*vx);
}

Triangle::Triangle(const Vec3& p1,
                   const Vec3& p2,
                   const Vec3& p3) :
    _flatShaded(true),
    _hasTexCoords(false)
{
    _faceNormal = CalcNormal(p1, p2, p3);
    _points[0] = p1;
    _points[1] = p2;
    _points[2] = p3;
}


void Triangle::set(const Vec3& p1,
                   const Vec3& p2,
                   const Vec3& p3)
{
    _faceNormal = CalcNormal(p1, p2, p3);
    _flatShaded = true;
    _points[0] = p1;
    _points[1] = p2;
    _points[2] = p3;
}

void ComputeBarycentricCoordinates(const Vec3& A,
                                 const Vec3& B,
                                 const Vec3& C,
                                 const Vec3& P,
                                 float& u,
                                 float& v)
{
    //the description of this algorithm can be found here:
    //http://www.blackpawn.com/texts/pointinpoly/default.html
    //or in this book: http://realtimecollisiondetection.net/
    Vec3 v0(C.x() - A.x(), C.y() - A.y(), C.z() - A.z());
    Vec3 v1(B.x() - A.x(), B.y() - A.y(), B.z() - A.z());
    Vec3 v2(P.x() - A.x(), P.y() - A.y(), P.z() - A.z());

    //Compute dot products
    float dot00 = Vec3::dot(v0, v0);
    float dot01 = Vec3::dot(v0, v1);
    float dot02 = Vec3::dot(v0, v2);
    float dot11 = Vec3::dot(v1, v1);
    float dot12 = Vec3::dot(v1, v2);

    //Compute barycentric coordinates
    float invDenom = 1.0f/((dot00 * dot11) - (dot01 * dot01));
    u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    v = (dot00 * dot12 - dot01 * dot02) * invDenom;
}

static bool TriContainsPoint(const Vec3& A,
                             const Vec3& B,
                             const Vec3& C,
                             const Vec3& P,
                             float& u, 
                             float& v)
{
    
    ComputeBarycentricCoordinates(A, B, C, P, u, v);

    //Check if point is in triangle
    static const float tolerance = 0.00001f;
    return (u+tolerance >= 0) && (v+tolerance >= 0) && ((u-tolerance + v-tolerance) <= 1);
}

bool Triangle::contains(const Vec3& testPoint, float& u, float& v) const
{
    return TriContainsPoint(_points[0], _points[1], _points[2], testPoint, u, v);
}

Vec3 Triangle::normal(const Vec3& pt) const
{
    if(_flatShaded)
    {
        return _faceNormal;
    }
    
    //curve shaded
    float u, v;
    ComputeBarycentricCoordinates(_points[0], _points[1], _points[2], pt, u, v);

    return normal(u, v);
}

Vec3 Triangle::normal(float u, float v) const
{
    if(_flatShaded)
    {
        return _faceNormal;
    }
    Vec3 V = (_normals[1] - _normals[0]) * v;
    Vec3 U = (_normals[2] - _normals[0]) * u;
    Vec3 interpNorm = _normals[0] + U + V;
    interpNorm.normalize();

    return interpNorm;
}

void Triangle::setNormals(const Vec3& n1, const Vec3& n2, const Vec3& n3)
{
    _flatShaded = false;
    _normals[0] = n1;
    _normals[1] = n2;
    _normals[2] = n3;
}

Vec2 Triangle::texCoord(const Vec3& pt) const
{
    float u, v;
    ComputeBarycentricCoordinates(_points[0], _points[1], _points[2], pt, u, v);

    return texCoord(u, v);
}

Vec2 Triangle::texCoord(float u, float v) const
{
    Vec2 V = (_texCoords[1] - _texCoords[0]) * v;
    Vec2 U = (_texCoords[2] - _texCoords[0]) * u;
    Vec2 interpUV = _texCoords[0] + U + V;

    return interpUV;
}

void Triangle::setTexCoords(const Vec2& uv1, const Vec2& uv2, const Vec2& uv3)
{
    _hasTexCoords = true;
    _texCoords[0] = uv1;
    _texCoords[1] = uv2;
    _texCoords[2] = uv3;
}

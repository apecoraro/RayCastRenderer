#include "Vec3.h"

using namespace cs500;

float Vec3::dot(const Vec3& a, const Vec3& b)
{
    return (a._v[0]*b._v[0]) + (a._v[1]*b._v[1]) + (a._v[2]*b._v[2]);
}

Vec3 Vec3::normalize(const Vec3& v)
{
    Vec3 copy(v);

    copy.normalize();

    return copy;
}

Vec3 Vec3::cross(const Vec3& lhs, const Vec3& rhs)
{
    return Vec3(lhs._v[1]*rhs._v[2]-lhs._v[2]*rhs._v[1],
                lhs._v[2]*rhs._v[0]-lhs._v[0]*rhs._v[2] ,
                lhs._v[0]*rhs._v[1]-lhs._v[1]*rhs._v[0]);
}
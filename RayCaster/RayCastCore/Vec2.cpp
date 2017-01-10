#include "Vec2.h"

using namespace cs500;

float Vec2::dot(const Vec2& a, const Vec2& b)
{
    return (a._v[0]*b._v[0]) + (a._v[1]*b._v[1]);
}

Vec2 Vec2::normalize(const Vec2& v)
{
    Vec2 copy(v);

    copy.normalize();

    return copy;
}

Vec2 Vec2::cross(const Vec2& lhs, const Vec2& rhs)
{
    return Vec2(lhs._v[1]*rhs._v[0]-lhs._v[1]*rhs._v[0],
                lhs._v[1]*rhs._v[0]-lhs._v[0]*rhs._v[1]);
}

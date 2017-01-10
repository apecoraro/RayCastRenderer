#include "Light.h"

using namespace cs500;

Light::Light() : _radius(-1.0f) {}

Light::Light(const Vec3& center,
              const Vec3& color,
              float radius) :
    _center(center),
    _color(color),
    _radius(radius)
{
}
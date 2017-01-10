#pragma once

#include "Referenced.h"
#include "Vec3.h"

namespace cs500
{
    class Light : public Referenced
    {
    public:
        Light();
        Light(const Vec3& center,
              const Vec3& color,
              float radius);

        const Vec3& center() const { return _center; }
        const Vec3& color() const { return _color; }
        float radius() const { return _radius; }

        void setCenter(const Vec3& center) { _center = center; }
        void setColor(const Vec3& color) { _color = color; }
        void setRadius(float radius) { _radius = radius; }
        
    private:
        Vec3 _center;
        Vec3 _color;
        float _radius;
    };
};
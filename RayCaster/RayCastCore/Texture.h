#pragma once

#include "Referenced.h"
#include "Vec3.h"

#include <string>
#include <vector>

namespace cs500
{
    class Texture : public Referenced
    {
    public:
        Texture(const std::string& file);

        const Vec3& getColor(float u, float v) const;
        const Vec3& getColor(size_t row, size_t col) const;

    private:
        ~Texture() {}

        typedef std::vector<Vec3> ImagePixels;

        ImagePixels _imagePixels;
        size_t _width;
        size_t _height;
    };
};
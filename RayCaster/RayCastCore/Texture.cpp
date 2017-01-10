#include "Texture.h"

#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace cs500;

Texture::Texture(const std::string& file)
{
    // Convert to a wchar_t*
    size_t convertedChars = 0;
    wchar_t *wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (file.length() + 1));
    mbstowcs_s(&convertedChars, wcstring, file.length()+1, file.c_str(), file.length());

    Bitmap bitmap(wcstring);

    _height = bitmap.GetHeight();
    _width = bitmap.GetWidth();

    for(UINT row = 0; row < bitmap.GetHeight(); ++row)
    {
        for(UINT col = 0; col < bitmap.GetWidth(); ++col)
        {
            Color color;
            bitmap.GetPixel(col, row, &color);

            _imagePixels.push_back(Vec3(color.GetRed()/255.0f, 
                                        color.GetGreen()/255.0f, 
                                        color.GetBlue()/255.0f));
        }
    }

    free(wcstring);
}

const Vec3& Texture::getColor(float u, float v) const
{
    size_t col = static_cast<size_t>(_width * u);
    size_t row = static_cast<size_t>(_height * v);

    return getColor(row, col);
}

const Vec3& Texture::getColor(size_t row, size_t col) const
{
    size_t index = (_width * row) + col;
    if(index < _imagePixels.size())
        return _imagePixels.at(index);
    else
    {
        static Vec3 invalidIndexColor(1.0f, 0.0f, 1.0f);

        return invalidIndexColor;
    }
}
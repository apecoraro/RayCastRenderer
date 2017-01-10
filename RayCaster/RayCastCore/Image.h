#pragma once

#include "Vec3.h"
#include "Referenced.h"

#include <vector>

namespace Gdiplus
{
    class Bitmap;
};

namespace cs500
{
    class Image : public Referenced
    {
    public:
        Image(int horizRes, int vertRes);

        void setPixel(int row, int col, 
                      unsigned char r, 
                      unsigned char g, 
                      unsigned char b);

        int horizontalResolution() const;
        int verticalResolution() const;

        void saveBitmap(const std::string& outputFile);
        Gdiplus::Bitmap* getBitmap() { return _pBitmap; }
    protected:
        virtual ~Image();
        
    private:
        Gdiplus::Bitmap* _pBitmap;
    };
};
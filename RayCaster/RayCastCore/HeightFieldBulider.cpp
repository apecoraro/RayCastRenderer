#include "HeightFieldBuilder.h"

#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace cs500;

HeightField* HeightFieldBuilder::CreateHeightField(const std::string& gridFile,
                                                   float maxZ,
                                                   float originX,
                                                   float originY,
                                                   float gridSpacingX,
                                                   float gridSpacingY,
                                                   float lodScalar/*=1.0f*/)
{
    size_t convertedChars = 0;
    wchar_t *wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (gridFile.length() + 1));
    mbstowcs_s(&convertedChars, wcstring, gridFile.length()+1, gridFile.c_str(), gridFile.length());

    Bitmap bitmap(wcstring);

    HeightField::GridPoints gridPoints;
    HeightField::GridTexCoords gridUVs;

    UINT height = bitmap.GetHeight();
    UINT width = bitmap.GetWidth();
    float deltaV = 1.0f / (static_cast<float>(height-1));
    float deltaU = 1.0f / (static_cast<float>(width-1));

    float v = 0.0f;
    for(UINT row = 0; row < height; ++row, v += deltaV)
    {
        std::vector<float> hfRow;
        std::vector<Vec2> uvRow;
        float u = 0.0f;
        for(UINT col = 0; col < width; ++col, u += deltaU)
        {
            Color color;
            bitmap.GetPixel(col, row, &color);

            BYTE red = color.GetRed();
            BYTE green = color.GetGreen();
            BYTE blue = color.GetBlue();

            float z = ((red + green + blue) / 3.0f) / 255.0f;

            z *= maxZ;

            hfRow.push_back(z);

            uvRow.push_back(Vec2(u, v));        
        }

        gridPoints.push_back(hfRow);
        gridUVs.push_back(uvRow);
    }

    free(wcstring);

    HeightField* pHeightField = new HeightField(gridPoints, 
                                                gridUVs, 
                                                originX, originY, 
                                                gridSpacingX, gridSpacingY, lodScalar);

    return pHeightField;
}
#pragma once

#include "HeightField.h"

namespace cs500
{
    class HeightFieldBuilder
    {
    public:
        static HeightField* CreateHeightField(const std::string& gridFile,
                                              float maxZ,
                                              float originX,
                                              float originY,
                                              float gridSpacingX,
                                              float gridSpacingY,
                                              float lodScalar=1.0f);
    };
};
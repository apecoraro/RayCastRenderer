#pragma once

#include "Vec3.h"

namespace cs500
{
    class Matrix
    {
    public:
        Matrix();
        Matrix(float a00, float a01, float a02,float a03,
               float a10, float a11, float a12,float a13,
               float a20, float a21, float a22,float a23,
               float a30, float a31, float a32,float a33);
        
        void set(float a00, float a01, float a02,float a03,
                 float a10, float a11, float a12,float a13,
                 float a20, float a21, float a22,float a23,
                 float a30, float a31, float a32,float a33);
         
        Vec3 transformPoint(const Vec3& v) const;
        Vec3 transformVector(const Vec3& v) const;

        float& operator()(int row, int col) { return _mat[row][col]; }
        float operator()(int row, int col) const { return _mat[row][col]; }

        bool invert(const Matrix& mat);
        void transpose();
    private:
        float _mat[4][4];
    };
};
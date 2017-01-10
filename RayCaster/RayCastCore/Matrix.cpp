#include "Matrix.h"

using namespace cs500;

Matrix::Matrix()
{
    set(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix::Matrix(float a00, float a01, float a02,float a03,
               float a10, float a11, float a12,float a13,
               float a20, float a21, float a22,float a23,
               float a30, float a31, float a32,float a33)
{
    set(a00, a01, a02, a03,
        a10, a11, a12, a13,
        a20, a21, a22, a23,
        a30, a31, a32, a33);
}


void Matrix::set(float a00, float a01, float a02,float a03,
                 float a10, float a11, float a12,float a13,
                 float a20, float a21, float a22,float a23,
                 float a30, float a31, float a32,float a33)
{
    _mat[0][0] = a00;
    _mat[0][1] = a01;
    _mat[0][2] = a02;
    _mat[0][3] = a03;

    _mat[1][0] = a10;
    _mat[1][1] = a11;
    _mat[1][2] = a12;
    _mat[1][3] = a13;

    _mat[2][0] = a20;
    _mat[2][1] = a21;
    _mat[2][2] = a22;
    _mat[2][3] = a23;

    _mat[3][0] = a30;
    _mat[3][1] = a31;
    _mat[3][2] = a32;
    _mat[3][3] = a33;
}
Vec3 Matrix::transformPoint(const Vec3& v) const
{
    float w = 1.0f;
    return Vec3((_mat[0][0]*v.x() + _mat[0][1]*v.y() + _mat[0][2]*v.z() + _mat[0][3]*w),
                (_mat[1][0]*v.x() + _mat[1][1]*v.y() + _mat[1][2]*v.z() + _mat[1][3]*w),
                (_mat[2][0]*v.x() + _mat[2][1]*v.y() + _mat[2][2]*v.z() + _mat[2][3]*w));
}

Vec3 Matrix::transformVector(const Vec3& v) const
{
    float w = 0.0f;
    return Vec3((_mat[0][0]*v.x() + _mat[0][1]*v.y() + _mat[0][2]*v.z() + _mat[0][3]*w),
                (_mat[1][0]*v.x() + _mat[1][1]*v.y() + _mat[1][2]*v.z() + _mat[1][3]*w),
                (_mat[2][0]*v.x() + _mat[2][1]*v.y() + _mat[2][2]*v.z() + _mat[2][3]*w));
}

inline float SGL_ABS(float a)
{
   return (a >= 0 ? a : -a);
}

#ifndef SGL_SWAP
#define SGL_SWAP(a,b,temp) ((temp)=(a),(a)=(b),(b)=((float)temp))
#endif

bool Matrix::invert(const Matrix& mat)
{
    if (&mat==this) {
       Matrix tm(mat);
       return invert(tm);
    }

    unsigned int indxc[4], indxr[4], ipiv[4];
    unsigned int i,j,k,l,ll;
    unsigned int icol = 0;
    unsigned int irow = 0;
    double temp, pivinv, dum, big;

    // copy in place this may be unnecessary
    *this = mat;

    for (j=0; j<4; j++) ipiv[j]=0;

    for(i=0;i<4;i++)
    {
       big=0.0;
       for (j=0; j<4; j++)
       {
          if (ipiv[j] != 1)
          {
             for (k=0; k<4; k++)
             {
                if (ipiv[k] == 0)
                {
                   if (SGL_ABS(operator()(j,k)) >= big)
                   {
                      big = SGL_ABS(operator()(j,k));
                      irow=j;
                      icol=k;
                   }
                }
                else if (ipiv[k] > 1)
                {
                   return false;
                }
             }
          }
       }
       ++(ipiv[icol]);
       if (irow != icol)
       {
          for (l=0; l<4; l++)
          {
              SGL_SWAP(operator()(irow,l),
                       operator()(icol,l),
                       temp);
          }
       }

       indxr[i]=irow;
       indxc[i]=icol;
       if (operator()(icol,icol) == 0)
       {
          return false;
       }

       pivinv = 1.0/operator()(icol,icol);
       operator()(icol,icol) = 1;
       for (l=0; l<4; l++) 
           operator()(icol,l) *= (float)pivinv;
       for (ll=0; ll<4; ll++)
       {
          if (ll != icol)
          {
             dum=operator()(ll,icol);
             operator()(ll,icol) = 0;
             for (l=0; l<4; l++) operator()(ll,l) -= operator()(icol,l)*(float)dum;
          }
       }
    }

    for (int lx=4; lx>0; --lx)
    {
       if (indxr[lx-1] != indxc[lx-1])
       {
           for (k=0; k<4; k++)
           { 
               SGL_SWAP(operator()(k,indxr[lx-1]),
                        operator()(k,indxc[lx-1]),temp); 
           }
       }
    }

    return true;
}

void Matrix::transpose()
{
    for(size_t row = 0; row < 4; ++row)
    {
        for(size_t col = row+1; col < 4; ++col)
        {
            float swap = _mat[row][col];
            _mat[row][col] = _mat[col][row];
            _mat[col][row] = swap;
        }
    }
}
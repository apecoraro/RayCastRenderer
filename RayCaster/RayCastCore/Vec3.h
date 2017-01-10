#pragma once

#include <math.h>

namespace cs500
{
    class Vec3
    {
    public:
        Vec3() { _v[0]=0.0f; _v[1]=0.0f; _v[2]=0.0f; }
        Vec3(float x, float y, float z) { _v[0]=x; _v[1]=y; _v[2]=z; }
        const float& x() const { return _v[0]; }
        const float& y() const { return _v[1]; }
        const float& z() const { return _v[2]; }

        float& x() { return _v[0]; }
        float& y() { return _v[1]; }
        float& z() { return _v[2]; }

        static Vec3 cross(const Vec3& lhs, const Vec3& rhs);
        static float dot(const Vec3& a, const Vec3& b);
        static Vec3 normalize(const Vec3& v);

        float length() const
        {
            return sqrtf( _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] );
        }

        float lengthSq() const
        {
            return _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2];
        }

        float normalize()
        {
            float norm = length();
            if (norm>0.0)
            {
                float inv = 1.0f/norm;
                _v[0] *= inv;
                _v[1] *= inv;
                _v[2] *= inv;
            }                
            return( norm );
        }

        bool operator == (const Vec3& v) const { return _v[0]==v._v[0] && _v[1]==v._v[1] && _v[2]==v._v[2]; }
        
        bool operator != (const Vec3& v) const { return _v[0]!=v._v[0] || _v[1]!=v._v[1] || _v[2]!=v._v[2]; }

        bool operator <  (const Vec3& v) const
        {
            if (_v[0]<v._v[0]) return true;
            else if (_v[0]>v._v[0]) return false;
            else if (_v[1]<v._v[1]) return true;
            else if (_v[1]>v._v[1]) return false;
            else return (_v[2]<v._v[2]);
        }

        float& operator [] (int i) { return _v[i]; }
        float operator [] (int i) const { return _v[i]; }

        const Vec3 operator * (float rhs) const
        {
            return Vec3(_v[0]*rhs, _v[1]*rhs, _v[2]*rhs);
        }

        Vec3& operator *= (float rhs)
        {
            _v[0]*=rhs;
            _v[1]*=rhs;
            _v[2]*=rhs;
            return *this;
        }

         const Vec3 operator * (const Vec3& rhs) const
        {
            return Vec3(_v[0]*rhs._v[0], _v[1]*rhs._v[1], _v[2]*rhs._v[2]);
        }

        Vec3& operator *= (const Vec3& rhs)
        {
            _v[0] *= rhs._v[0];
            _v[1] *= rhs._v[1];
            _v[2] *= rhs._v[2];
            return *this;
        }

        const Vec3 operator / (float rhs) const
        {
            return Vec3(_v[0]/rhs, _v[1]/rhs, _v[2]/rhs);
        }

        Vec3& operator /= (float rhs)
        {
            _v[0]/=rhs;
            _v[1]/=rhs;
            _v[2]/=rhs;
            return *this;
        }

        const Vec3 operator + (const Vec3& rhs) const
        {
            return Vec3(_v[0]+rhs._v[0], _v[1]+rhs._v[1], _v[2]+rhs._v[2]);
        }

        Vec3& operator += (const Vec3& rhs)
        {
            _v[0] += rhs._v[0];
            _v[1] += rhs._v[1];
            _v[2] += rhs._v[2];
            return *this;
        }

        const Vec3 operator - (const Vec3& rhs) const
        {
            return Vec3(_v[0]-rhs._v[0], _v[1]-rhs._v[1], _v[2]-rhs._v[2]);
        }

        Vec3& operator -= (const Vec3& rhs)
        {
            _v[0]-=rhs._v[0];
            _v[1]-=rhs._v[1];
            _v[2]-=rhs._v[2];
            return *this;
        }

        const Vec3 operator - () const
        {
            return Vec3 (-_v[0], -_v[1], -_v[2]);
        }
    private:
        float _v[3];
    };
};
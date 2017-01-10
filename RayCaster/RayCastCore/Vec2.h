#pragma once

#include <math.h>

namespace cs500
{
    class Vec2
    {
    public:
        Vec2() { _v[0]=0.0f; _v[1]=0.0f; }
        Vec2(float x, float y) { _v[0]=x; _v[1]=y; }
        const float& x() const { return _v[0]; }
        const float& y() const { return _v[1]; }

        float& x() { return _v[0]; }
        float& y() { return _v[1]; }

        static Vec2 cross(const Vec2& lhs, const Vec2& rhs);
        static float dot(const Vec2& a, const Vec2& b);
        static Vec2 normalize(const Vec2& v);

        float length() const
        {
            return sqrtf( _v[0]*_v[0] + _v[1]*_v[1] );
        }

        float lengthSq() const
        {
            return _v[0]*_v[0] + _v[1]*_v[1];
        }

        float normalize()
        {
            float norm = length();
            if (norm>0.0)
            {
                float inv = 1.0f/norm;
                _v[0] *= inv;
                _v[1] *= inv;
            }                
            return( norm );
        }

        bool operator == (const Vec2& v) const { return _v[0]==v._v[0] && _v[1]==v._v[1]; }
        
        bool operator != (const Vec2& v) const { return _v[0]!=v._v[0] || _v[1]!=v._v[1]; }

        bool operator <  (const Vec2& v) const
        {
            if (_v[0]<v._v[0]) return true;
            else if (_v[0]>v._v[0]) return false;
            else return (_v[1]<v._v[1]);
        }

        float& operator [] (int i) { return _v[i]; }
        float operator [] (int i) const { return _v[i]; }

        const Vec2 operator * (float rhs) const
        {
            return Vec2(_v[0]*rhs, _v[1]*rhs);
        }

        Vec2& operator *= (float rhs)
        {
            _v[0]*=rhs;
            _v[1]*=rhs;
            return *this;
        }

         const Vec2 operator * (const Vec2& rhs) const
        {
            return Vec2(_v[0]*rhs._v[0], _v[1]*rhs._v[1]);
        }

        Vec2& operator *= (const Vec2& rhs)
        {
            _v[0] *= rhs._v[0];
            _v[1] *= rhs._v[1];
            return *this;
        }

        const Vec2 operator / (float rhs) const
        {
            return Vec2(_v[0]/rhs, _v[1]/rhs);
        }

        Vec2& operator /= (float rhs)
        {
            _v[0]/=rhs;
            _v[1]/=rhs;
            return *this;
        }

        const Vec2 operator + (const Vec2& rhs) const
        {
            return Vec2(_v[0]+rhs._v[0], _v[1]+rhs._v[1]);
        }

        Vec2& operator += (const Vec2& rhs)
        {
            _v[0] += rhs._v[0];
            _v[1] += rhs._v[1];
            return *this;
        }

        const Vec2 operator - (const Vec2& rhs) const
        {
            return Vec2(_v[0]-rhs._v[0], _v[1]-rhs._v[1]);
        }

        Vec2& operator -= (const Vec2& rhs)
        {
            _v[0]-=rhs._v[0];
            _v[1]-=rhs._v[1];
            return *this;
        }

        const Vec2 operator - () const
        {
            return Vec2 (-_v[0], -_v[1]);
        }
    private:
        float _v[2];
    };
};

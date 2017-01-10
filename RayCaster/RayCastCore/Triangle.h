#pragma once

#include "Vec3.h"
#include "Vec2.h"

namespace cs500
{
    class Triangle
    {
    public:
        Triangle(const Vec3& p1,
                 const Vec3& p2,
                 const Vec3& p3);

        const Vec3& points(size_t index) const { return _points[index]; }
        
        Vec3 normal(const Vec3& pt) const;
        Vec3 normal(float u, float v) const;
        const Vec3& normal() const { return _faceNormal; }

        void setNormals(const Vec3& n1, const Vec3& n2, const Vec3& n3);

        Vec2 texCoord(const Vec3& pt) const;
        Vec2 texCoord(float u, float v) const;

        void setTexCoords(const Vec2& uv1, const Vec2& uv2, const Vec2& uv3);

        void set(const Vec3& p1,
                 const Vec3& p2,
                 const Vec3& p3);

        bool contains(const Vec3& point, float& u, float& v) const;

        bool isFlatShaded() const { return _flatShaded; }
        bool hasTexCoords() const { return _hasTexCoords; }
    private:
        Vec3 _points[3];
        Vec3 _faceNormal;
        Vec3 _normals[3];
        bool _flatShaded;

        bool _hasTexCoords;
        Vec2 _texCoords[3];
    };
};
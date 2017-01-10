#pragma once

#include "Node.h"
#include "Triangle.h"
#include "Vec3.h"
#include "Ray.h"

#include <vector>

namespace cs500
{
    class Polygon : public Node
    {
    public:
        Polygon() {}

        void addTriangle(const Triangle& triangle);
        void addTriangle(const Vec3& p1,
                         const Vec3& p2,
                         const Vec3& p3);

        typedef std::vector<Triangle> Triangles;
        size_t getNumTriangles() const { return _triangles.size(); }

        const Triangle& getTriangle(size_t index) const { return _triangles.at(index); }
        Triangle& getTriangle(size_t index) { return _triangles.at(index); }

        virtual bool intersect(const Ray& ray, 
                               IntersectionData& data) const;

        virtual BoundingSphere computeBoundingSphere() const;
        virtual BoundingBox computeBoundingBox() const;

        virtual bool hasThickness() const { return false; }
    private:
        Triangles _triangles;
    };
};
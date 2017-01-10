#pragma once

#include "Node.h"
#include "Vec3.h"
#include "Matrix.h"

namespace cs500
{
    class Ellipsoid : public Node
    {
    public:
        Ellipsoid() {}

        Ellipsoid(const Vec3& center,
                  const Vec3& uAxis,
                  const Vec3& vAxis,
                  const Vec3& wAxis) :
            _center(center), _uAxis(uAxis), _vAxis(vAxis), _wAxis(wAxis) 
        {
            computeIntersectionMatrices();
        }

        const Vec3& center() const { return _center; }
        const Vec3& uAxis() const { return _uAxis; }
        const Vec3& vAxis() const { return _vAxis; }
        const Vec3& wAxis() const { return _wAxis; }
        const Matrix& unitSphereToEllipsoidMatrix() const { return _unitSphereToEllipsoidMatrix; }
        const Matrix& unitSphereNormToEllipsoidNormMatrix() const { return _unitSphereNormToEllipsoidNormMatrix; }
        const Matrix& ellipsoidToUnitSphereMatrix() const { return _ellipsoidToUnitSphereMatrix; }

        void set(const Vec3& center,
                  const Vec3& uAxis,
                  const Vec3& vAxis,
                  const Vec3& wAxis)
        {
            _center = center;
            _uAxis = uAxis;
            _vAxis = vAxis;
            _wAxis = wAxis;

            computeIntersectionMatrices();
            _boundsNeedUpdating = true;
        }

        virtual bool intersect(const Ray& ray, 
                               IntersectionData& data) const;

        virtual BoundingSphere computeBoundingSphere() const;
        virtual BoundingBox computeBoundingBox() const;

    protected:
        void computeIntersectionMatrices();

    private:
        Vec3 _center;
        Vec3 _uAxis;
        Vec3 _vAxis;
        Vec3 _wAxis;

        Matrix _unitSphereToEllipsoidMatrix;//used for intersection testing
        Matrix _unitSphereNormToEllipsoidNormMatrix;
        Matrix _ellipsoidToUnitSphereMatrix;//used for intersection testing
    };
};
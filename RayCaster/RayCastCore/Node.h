#pragma once

#include "Ray.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Referenced.h"
#include "SurfaceProperties.h"
#include "SmartPtr.h"
#include "Camera.h"
#include "BoundingVolumes.h"

namespace cs500
{
    class Node : public Referenced
    {
    public:
        Node();

        virtual BoundingSphere computeBoundingSphere() const = 0;
        const BoundingSphere& boundingSphere() const
        {
            if(_boundsNeedUpdating || !_boundingSphere.valid())
            {
                _boundingSphere.expandBy(computeBoundingSphere());
                if(_boundsNeedUpdating)
                    _boundingBox.expandBy(computeBoundingBox());

                _boundsNeedUpdating = false;
            }

            return _boundingSphere;
        }

        virtual BoundingBox computeBoundingBox() const = 0;
        const BoundingBox& boundingBox() const
        {
            if(_boundsNeedUpdating || !_boundingBox.valid())
            {
                if(_boundsNeedUpdating)
                    _boundingSphere.expandBy(computeBoundingSphere());
                    
                _boundingBox.expandBy(computeBoundingBox());

                _boundsNeedUpdating = false;
            }

            return _boundingBox;
        }

        virtual void computeProjectedSize(const Camera& camera, int horizRes)
        {
            //only the group and the height field implements this at the moment
        }


        struct IntersectionData
        {
            float isectTVal; 
            Vec3 isectPt;
            Vec3 isectNorm;
            Vec3 shadingNorm;
            Vec2 texCoords;
            bool isCurveShaded;
            bool hasTexCoords;
            SmartPtr<const Node> spIntersectedObject;
            IntersectionData() : isectTVal(FLT_MAX), isCurveShaded(false), hasTexCoords(false) {}
        };

        virtual bool intersect(const Ray& ray, 
                               IntersectionData& data) const = 0;

        const SurfaceProperties* surfaceProperties() const { return _spSurfaceProps.get(); }
        SurfaceProperties* surfaceProperties() { return _spSurfaceProps.get(); }

        void setSurfaceProperties(SurfaceProperties* pSurfProps) { _spSurfaceProps = pSurfProps; }

        virtual bool hasThickness() const { return true; }
        virtual bool useSpatialTree() const { return true; }
    protected:
        virtual ~Node() {}

        mutable bool _boundsNeedUpdating;
    private:
        SmartPtr<SurfaceProperties> _spSurfaceProps;
        mutable BoundingSphere _boundingSphere;
        mutable BoundingBox _boundingBox;
    };
};
#pragma once

#include "Vec3.h"

namespace cs500
{
    class BoundingSphere
    {
    public:
        BoundingSphere();
        BoundingSphere(const Vec3& center,
                       float radius);
        ~BoundingSphere();

        const Vec3& center() const;
        float radius() const;
        float radiusSq() const;

        void setRadius(float r) { _radius = r; _radiusSq = r * r; }
        void setCenter(const Vec3& c) { _center = c; }

        bool valid() const { return _radius >= 0.0f; }
        bool contains(const Vec3& pos) const;

        void expandBy(const BoundingSphere& sphere);
        void expandBy(const Vec3& point);
    private:

        Vec3 _center;
        float _radius;
        float _radiusSq;
    };

    class BoundingBox
    {
    public:
	    BoundingBox() : _corner(0.0f, 0.0f, 0.0f), _sizeVec(-1.0f, -1.0f, -1.0f) {};
	    BoundingBox(const Vec3& corner, const Vec3& sizeVec ) : _corner(corner), _sizeVec(sizeVec) {};
	    
        const Vec3& getCorner() const { return _corner; }
        Vec3& getCorner() { return _corner; }

	    const Vec3& getSizeVec() const { return _sizeVec; }
	    Vec3& getSizeVec() { return _sizeVec; }

        Vec3 getFarCorner() const { return _corner + _sizeVec; }

	    bool intersect(const BoundingBox& bb) const
	    {
		    const Vec3& v1 = bb.getCorner();
            Vec3 v2 = bb.getFarCorner();
		    const Vec3& v3 = _corner;
            Vec3 v4 = _corner + _sizeVec;
		    return ((v4.x() >= v1.x()) && (v3.x() <= v2.x()) && // x-axis overlap
				    (v4.y() >= v1.y()) && (v3.y() <= v2.y()) && // y-axis overlap
				    (v4.z() >= v1.z()) && (v3.z() <= v2.z()));   // z-axis overlap
	    }

	    bool contains(const Vec3& point) const
	    {
		    const Vec3& v1 = _corner;
            Vec3 v2 = _corner + _sizeVec;
		    return ((point.x() >= v1.x()) && (point.x() <= v2.x()) &&
				    (point.y() >= v1.y()) && (point.y() <= v2.y()) &&
				    (point.z() >= v1.z()) && (point.z() <= v2.z()));
	    }

	    float width() const { return _sizeVec.x(); }
	    float height() const { return _sizeVec.y(); }
	    float depth() const { return _sizeVec.z(); }

        void expandBy(const BoundingBox& box);
        void expandBy(const Vec3& point);

        bool valid() const { return _sizeVec.x() >= 0.0f &&
                                    _sizeVec.y() >= 0.0f &&
                                    _sizeVec.z() >= 0.0f; }
    private:
	    Vec3 _corner;
        Vec3 _sizeVec;
    };
};
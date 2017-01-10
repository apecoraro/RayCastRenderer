#pragma once

#include "Group.h"
#include "SmartPtr.h"

#include <map>

namespace cs500
{
    class SphereTree : public Group
    {
    public:
        SphereTree() : _minChildren(4), _isLeaf(false) {}

        void build(Group* pSceneObjectRoot);

        virtual bool intersect(const Ray& ray, 
                               IntersectionData& data) const;
    protected:
        
        typedef std::map<Node*, float> DistanceMap;
        typedef std::map<Node*, DistanceMap> NodeDistanceMap;

        void subdivide(const NodeDistanceMap& nodeDistMap);
        void findAndDetachFarthestObjects(const NodeDistanceMap& nodeDistMap,
                                          SmartPtr<Node>& spObj1, 
                                          SmartPtr<Node>& spObj2);
        void buildNodeDistanceMap(NodeDistanceMap& nodeDistMap);
    private:
        size_t _minChildren;
        bool _isLeaf;
    };
};
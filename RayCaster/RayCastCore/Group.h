#pragma once

#include "Node.h"
#include "SmartPtr.h"
#include "Sphere.h"

#include <vector>

namespace cs500
{
    class Group : public Node
    {
    public:
        Group() {}
        typedef std::vector< SmartPtr<Node> > Nodes;
        void addChild(Node* pChild);
        const Node* getChild(size_t index) const;
        Node* getChild(size_t index);
        size_t getNumChildren() const { return _children.size(); }

        void removeChild(size_t index)
        {
            if(index < _children.size())
                _children.erase(_children.begin()+index);
        }

        void removeChild(const Node* pChild);
        void clearChildren();

        virtual bool intersect(const Ray& ray, 
                               IntersectionData& data) const;

        virtual BoundingSphere computeBoundingSphere() const;
        virtual BoundingBox computeBoundingBox() const;

        virtual void computeProjectedSize(const Camera& camera, int horizRes);
    protected:
        virtual ~Group() {}
    private:
        Nodes _children;
    };
};
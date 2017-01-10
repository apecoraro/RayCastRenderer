#include "Group.h"

#include <float.h>

using namespace cs500;

void Group::addChild(Node* pChild)
{
    _children.push_back(pChild);
    _boundsNeedUpdating = true;
}

const Node* Group::getChild(size_t index) const
{
    return _children.at(index);
}

Node* Group::getChild(size_t index)
{
    return _children.at(index);
}

BoundingSphere Group::computeBoundingSphere() const
{
    BoundingSphere bounds;
    for(size_t childIndex = 0;
        childIndex < _children.size();
        ++childIndex)
    {
        Node* pNode = _children.at(childIndex).get();
        
        bounds.expandBy(pNode->boundingSphere());
    }

    return bounds;
}

BoundingBox Group::computeBoundingBox() const
{
    BoundingBox bounds;
    for(size_t childIndex = 0;
        childIndex < _children.size();
        ++childIndex)
    {
        Node* pNode = _children.at(childIndex).get();
        
        bounds.expandBy(pNode->boundingBox());
    }

    return bounds;
}

void Group::computeProjectedSize(const Camera& camera, int horizRes)
{
    for(size_t childIndex = 0;
        childIndex < _children.size();
        ++childIndex)
    {
        Node* pNode = _children.at(childIndex).get();
        pNode->computeProjectedSize(camera, horizRes);
    }
}

void Group::removeChild(const Node* pRmChild)
{
    for(Nodes::iterator itr = _children.begin();
        itr != _children.end();
        ++itr)
    {
        Node* pNode = itr->get();
        if(pNode == pRmChild)
        {
            _children.erase(itr);
            break;
        }
    }
}

void Group::clearChildren()
{
    _children.clear();
}

bool Group::intersect(const Ray& ray, 
                      IntersectionData& data) const
{
    bool success = false;
    IntersectionData curData;
    //TODO intersect with this group's bounding sphere first
    for(size_t childIndex = 0;
        childIndex < _children.size();
        ++childIndex)
    {
        Node* pNode = _children.at(childIndex).get();
        if(pNode->intersect(ray, curData))
        {
            success = true;
            if(curData.isectTVal < data.isectTVal)
            {
                data = curData;
            }
        }
    }

    return success;
}
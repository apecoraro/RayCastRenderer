#include "SphereTree.h"

#include "Vec3.h"

#include "SmartPtr.h"

using namespace cs500;

void SphereTree::build(Group* pScene)
{
    for(size_t i = 0; i < pScene->getNumChildren(); )
    {
        Node* pChild = pScene->getChild(i);
        if(pChild->useSpatialTree())
        {
            addChild(pChild);

            pScene->removeChild(i);
        }
        else
            ++i;
    }

    NodeDistanceMap nodeDistMap;
    buildNodeDistanceMap(nodeDistMap);

    subdivide(nodeDistMap);

    boundingSphere();//compute this now so we don't have to do it during the ray tracing part
}

void SphereTree::buildNodeDistanceMap(NodeDistanceMap& nodeDistMap)
{
    //initialize
    for(size_t childIndex1 = 0;
        childIndex1 < getNumChildren();
        ++childIndex1)
    {
        nodeDistMap.insert(std::make_pair(getChild(childIndex1), DistanceMap()));
    }

    for(size_t childIndex1 = 0;
        childIndex1 < getNumChildren();
        ++childIndex1)
    {
        Node* pChild1 = getChild(childIndex1);

        const BoundingSphere& sphere1 = pChild1->boundingSphere();

        DistanceMap& distMap1 = nodeDistMap.find(pChild1)->second;

        for(size_t childIndex2 = childIndex1 + 1;
            childIndex2 < getNumChildren();
            ++childIndex2)
        {
            Node* pChild2 = getChild(childIndex2);
            
            const BoundingSphere& sphere2 = pChild2->boundingSphere();

            Vec3 distVec = sphere1.center() - sphere2.center();
            float distSq = distVec.lengthSq();

            distMap1.insert(std::make_pair(pChild2, distSq));

            DistanceMap& distMap2 = nodeDistMap.find(pChild2)->second;

            distMap2.insert(std::make_pair(pChild1, distSq));
        }
    }
}

void SphereTree::findAndDetachFarthestObjects(const NodeDistanceMap& nodeDistMap,
                                              SmartPtr<Node>& spObj1, 
                                              SmartPtr<Node>& spObj2)
{
    float maxDistSq = -FLT_MAX;
    for(size_t childIndex1 = 0;
        childIndex1 < getNumChildren();
        ++childIndex1)
    {
        Node* pChild1 = getChild(childIndex1);

        const DistanceMap& c1DistMap = nodeDistMap.find(pChild1)->second;

        for(size_t childIndex2 = childIndex1 + 1;
            childIndex2 < getNumChildren();
            ++childIndex2)
        {
            Node* pChild2 = getChild(childIndex2);

            float distSq = c1DistMap.find(pChild2)->second;

            if(distSq > maxDistSq)
            {
                maxDistSq = distSq;
                spObj1 = pChild1;
                spObj2 = pChild2;
            }
        }
    }

    removeChild(spObj1.get());
    removeChild(spObj2.get());
}

void SphereTree::subdivide(const NodeDistanceMap& nodeDistMap)
{
    if(getNumChildren() <= _minChildren)
    {
        _isLeaf = true;
        return;
    }

    SmartPtr<Node> spObj1;
    SmartPtr<Node> spObj2;

    findAndDetachFarthestObjects(nodeDistMap, spObj1, spObj2);

    SmartPtr<SphereTree> spTree1 = new SphereTree();
    spTree1->addChild(spObj1.get());

    SmartPtr<SphereTree> spTree2 = new SphereTree();
    spTree2->addChild(spObj2.get());

    for(size_t childIndex = 0;
        childIndex < getNumChildren();
        ++childIndex)
    {
        Node* pChild = getChild(childIndex);

        const DistanceMap& childDistMap = nodeDistMap.find(pChild)->second;

        float distSq1 = childDistMap.find(spObj1.get())->second;
        float distSq2 = childDistMap.find(spObj2.get())->second;

        if(distSq1 < distSq2)
            spTree1->addChild(pChild);
        else
            spTree2->addChild(pChild);
    }

    clearChildren();

    addChild(spTree1.get());
    addChild(spTree2.get());

    spTree1->subdivide(nodeDistMap);
    spTree2->subdivide(nodeDistMap);
}

bool SphereTree::intersect(const Ray& ray, 
                           IntersectionData& data) const
{
    if(!_isLeaf)
    {
        bool success = false;
        IntersectionData curData;

        size_t num = getNumChildren();
        for(size_t childIndex = 0;
            childIndex < num;
            ++childIndex)
        {
            const Node* pChild = getChild(childIndex);
            const BoundingSphere& sphere = pChild->boundingSphere();
            float t;
            if(ray.intersect(sphere, t))
            {
                if(pChild->intersect(ray, curData))
                {
                    success = true;
                    if(curData.isectTVal < data.isectTVal)
                    {
                        data = curData;
                    }
                }
            }
        }

        return success;
    }
    else
    {
        return Group::intersect(ray, data);
    }
}
#include "KDTree.h"

#include <assert.h>

using namespace cs500;

static const Vec3 k_XAxis(1.0f, 0.0f, 0.0f);
static const Vec3 k_YAxis(0.0f, 1.0f, 0.0f);
static const Vec3 k_ZAxis(0.0f, 0.0f, 1.0f);

KDTreeNode::KDTreeNode() :
    _axis(X_AXIS)
{
}

void KDTreeNode::setSplitPlane(SplitAxis splitAxis, float splitPos)
{
    _axis = splitAxis;
    switch(_axis)
    {
    case X_AXIS:
        {
            _splitPlane = Plane(Vec3(splitPos, 0.0f, 0.0f), k_XAxis);
            break;
        }
    case Y_AXIS:
        {
            _splitPlane = Plane(Vec3(0.0f, splitPos, 0.0f), k_YAxis);
            break;
        }
    case Z_AXIS:
    default:
        {
            _splitPlane = Plane(Vec3(0.0f, 0.0f, splitPos), k_ZAxis);
            break;
        }
    }
}

bool KDTreeNode::intersect(const Ray& ray, Node::IntersectionData& data) const
{
    if(isLeaf())
    {
        bool success = false;
        Node::IntersectionData curData;
        for(size_t oIndex = 0;
            oIndex < _objectList.size();
            ++oIndex)
        {
            Node* pNode = _objectList.at(oIndex).get();

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
    else
    {
        KDTreeNode* pNearChild = _spLeftChild.get();
        KDTreeNode* pFarChild = _spRightChild.get();
        //determine side that ray starts
        float sideVal = _splitPlane.eval(ray.start());
        if(sideVal > 0.0f)
        {
            pNearChild = _spRightChild.get();
            pFarChild = _spLeftChild.get();
        }
        
        float t;
        if(ray.intersect(_splitPlane, t))
        {
            if(pNearChild->intersect(ray, data))
                return true;
            else
            {
                Ray farRay(ray.getPoint(t), ray.direction());
                return pFarChild->intersect(farRay, data);
            }
        }
        else//did not intersect the plane so must be paralell - can only intersect near child
            return pNearChild->intersect(ray, data);
    }
}

KDTree::KDTree() : _spRoot(new KDTreeNode())
{
}

BoundingSphere KDTree::computeBoundingSphere() const
{
    //fake this for now since these functions are only used by the KDTree at the moment
    return BoundingSphere(Vec3(0.0f, 0.0f, 0.0f), 9999.9f);
}

BoundingBox KDTree::computeBoundingBox() const
{
    //fake this for now since these functions are only used by the KDTree at the moment
    return BoundingBox(Vec3(-9999.9f, -9999.9f, -9999.9f),
                       Vec3(9999.9f, 9999.9f, 9999.9f));
}

void KDTree::build(Group* pScene)
{
    const BoundingBox& sceneBBox = pScene->boundingBox();

    for(size_t i = 0; i < pScene->getNumChildren(); )
    {
        Node* pChild = pScene->getChild(i);
        if(pChild->useSpatialTree())
        {
            _spRoot->addObject(pChild);
            pScene->removeChild(i);
        }
        else
            ++i;
    }

    size_t numObjs = _spRoot->numObjects();

    _splitPool.resize(numObjs * 2 + 8);

    subdivide(_spRoot.get(), sceneBBox, 0, numObjs);
}

bool KDTree::intersect(const Ray& ray,
                       Node::IntersectionData& data) const
{
    return _spRoot->intersect(ray, data);
}

void KDTree::insertSplitPos(float splitPos)
{
    // insert a split position candidate in the list if unique
    SplitPosList::iterator entry = _splitPool.begin();
    entry->pos = splitPos;
    entry->leftCount = 0;
    entry->rightCount = 0;
    //insert sorted low to high
    for(SplitPosList::iterator itr = _splitList.begin(); 
        itr != _splitList.end(); ++itr)
    {
        if(splitPos > itr->pos)
        {
            SplitPosList::iterator next = itr;
            ++next;
            if(next == _splitList.end())
                break;
            else if(splitPos < next->pos)
            {
                _splitList.splice(next, _splitPool, entry);
                return;
            }
            else if(splitPos == next->pos)
                return;
        }
        else if (splitPos == itr->pos)
            return;
    }
    _splitList.splice(_splitList.end(), _splitPool, entry);
   
}

float KDTree::computeCostAndBestSplitPosOfAxis(KDTreeNode::SplitAxis axis, 
                                              const KDTreeNode::ObjectList& objList, 
                                              const BoundingBox& bbox,
                                              size_t numObjs,
                                              float& bestPos,
                                              BoundingBox& bboxBestLeft,//bounding box of left child with best split
                                              BoundingBox& bboxBestRight,//bounding box of right child with best split)
                                              std::vector<float>& objMinPositions,
                                              std::vector<float>& objMaxPositions)
{
    //const Vec3& sizeVec = bbox.getSizeVec();
    //
    //if((sizeVec.x() >= sizeVec.y()) &&
    //   (sizeVec.x() >= sizeVec.z()))
    //{
    //    pNode->setSplitAxis(KDTreeNode::X_AXIS);
    //}
    //else if((sizeVec.y() >= sizeVec.x()) &&
    //        (sizeVec.y() >= sizeVec.z()))
    //{
    //    pNode->setSplitAxis(KDTreeNode::Y_AXIS);
    //}
    //else
    //    pNode->setSplitAxis(KDTreeNode::Z_AXIS);

    //pNode->setSplitAxis(axis);

    //KDTreeNode::SplitAxis axis = pNode->getSplitAxis();

    float minPos = bbox.getCorner()[(int)axis];
    float maxPos = bbox.getFarCorner()[(int)axis];

    buildSplitPosCandidates(objList, axis, minPos, maxPos, objMinPositions, objMaxPositions);

    std::vector<bool> goRight(numObjs, true);

    //count number of objects on left and right of each split plane
    BoundingBox bboxLeft = bbox;//left child bounding box
    BoundingBox bboxRight = bbox;//right child bounding box
    float bboxLeftMinPos = bboxLeft.getCorner()[(int)axis];
    float bboxRightMaxPos = bboxRight.getCorner()[(int)axis] + bboxRight.getSizeVec()[(int)axis];
    for(SplitPosList::iterator itr = _splitList.begin();
        itr != _splitList.end();
        ++itr)
    {
        SplitPos& splitPos = *itr;
        bboxRight.getCorner()[(int)axis] = splitPos.pos;
        bboxRight.getSizeVec()[(int)axis] = maxPos - splitPos.pos;

        bboxLeft.getSizeVec()[(int)axis] = splitPos.pos - minPos;

        float bboxLeftMaxPos = bboxLeft.getCorner()[(int)axis] + bboxLeft.getSizeVec()[(int)axis];

        float bboxRightMinPos = bboxRight.getCorner()[(int)axis];

        for(KDTreeNode::ObjectList::const_iterator oitr = objList.begin();
            oitr != objList.end();
            ++oitr)
        {
            size_t i = oitr - objList.begin();
            if(goRight[i] == true)
            {
                const Node* pObj = *oitr;

                if ((objMinPositions[i] <= bboxLeftMaxPos) && (objMaxPositions[i] >= bboxLeftMinPos))
                {
                    if (pObj->boundingBox().intersect(bboxLeft)) 
                        ++splitPos.leftCount;
                }

                if ((objMinPositions[i] <= bboxRightMaxPos) && (objMaxPositions[i] >= bboxRightMinPos))
                {
                    if (pObj->boundingBox().intersect(bboxRight)) 
                        ++splitPos.rightCount;
                    else 
                        goRight[i] = false;
                }
            }
            else 
                ++splitPos.leftCount;
        }
        
    }
    
    float curNodeSurfArea = 0.5f / 
        (bbox.width() * bbox.depth() + bbox.width() * bbox.height() + bbox.depth() * bbox.height());

    float lowestCost;
    computeBestSplitPlanePos(lowestCost, bestPos, 
                             bboxBestLeft, bboxBestRight, 
                             minPos, maxPos, 
                             bboxLeft, bboxRight, 
                             _splitList, curNodeSurfArea,
                             axis);

    return lowestCost;
}
        
void KDTree::subdivide(KDTreeNode* pNode, const BoundingBox& bbox, 
                       size_t depth, size_t numObjs)
{
    if(depth >= 10)
        return;
    
    KDTreeNode::ObjectList& objList = pNode->getObjects();

    float costNoSplit = numObjs * 1.0f;
    float lowestCost = FLT_MAX;
    float bestPos = 0.0f;
    KDTreeNode::SplitAxis lowestAxisCost = KDTreeNode::X_AXIS;
    BoundingBox bboxBestLeft, bboxBestRight;
    std::vector<float> objMinPositions;//the min value of the bbox of each object on the selected axis
    std::vector<float> objMaxPositions;//the max value of the bbox of each object on the selected axis
    std::vector<float> axisObjMinPos(numObjs);//allocate once
    std::vector<float> axisObjMaxPos(numObjs);//allocate once
    for(int i = 0; i < 3; ++i)//try all three axes and choose the one with smallest cost to split
    {
        float axisPos;
        BoundingBox axisBestLeft, axisBestRight;
        
        float axisCost = computeCostAndBestSplitPosOfAxis((KDTreeNode::SplitAxis)i, objList, bbox, numObjs, 
                                                          axisPos, 
                                                          axisBestLeft, axisBestRight,
                                                          axisObjMinPos, axisObjMaxPos);
        if(axisCost < lowestCost && axisCost < costNoSplit)
        {
            lowestCost = axisCost;
            lowestAxisCost = (KDTreeNode::SplitAxis)i;

            bestPos = axisPos;

            bboxBestLeft = axisBestLeft;
            bboxBestRight = axisBestRight;

            objMinPositions.swap(axisObjMinPos);
            objMaxPositions.swap(axisObjMaxPos);

            axisObjMinPos.resize(numObjs);//reallocate
            axisObjMaxPos.resize(numObjs);//reallocate
        }
    }

    if(lowestCost > costNoSplit)
        return;

    //free these two vectors since they might be really large
    {
        std::vector<float> empty1;
        empty1.swap(axisObjMinPos);

        std::vector<float> empty2;
        empty2.swap(axisObjMaxPos);
    }

    pNode->setSplitPlane(lowestAxisCost, bestPos);

    KDTreeNode* pLeft = new KDTreeNode();
    KDTreeNode* pRight = new KDTreeNode();

    int leftCount = 0;
    int rightCount = 0;
    float bboxLeftBestMinPos = bboxBestLeft.getCorner()[(int)lowestAxisCost];
    float bboxRightBestMaxPos = bboxBestRight.getCorner()[(int)lowestAxisCost] + bboxBestRight.getSizeVec()[(int)lowestAxisCost];
    float bboxLeftBestMaxPos = bboxBestLeft.getCorner()[(int)lowestAxisCost] + bboxBestLeft.getSizeVec()[(int)lowestAxisCost];
    float bboxRightBestMinPos = bboxBestRight.getCorner()[(int)lowestAxisCost];
    for(KDTreeNode::ObjectList::iterator oitr = objList.begin();
        oitr != objList.end();
        ++oitr)
    {
        size_t i = oitr - objList.begin();
        bool objAddedToAtLeastOne = false;

        Node* pObj = *oitr;
        //total++;
        if ((objMaxPositions[i] >= bboxLeftBestMinPos) && (objMinPositions[i] <= bboxLeftBestMaxPos))
        {
            //if (pObj->boundingBox().intersect(bboxBestLeft)) 
            {
                pLeft->addObject(pObj);
                ++leftCount;
                objAddedToAtLeastOne = true;
            }
        }

        if ((objMaxPositions[i] >= bboxRightBestMinPos) && (objMinPositions[i] <= bboxRightBestMaxPos))
        {
            //if (pObj->boundingBox().intersect(bboxBestRight)) 
            {
                pRight->addObject(pObj);
                ++rightCount;
                objAddedToAtLeastOne = true;
            }
        }

        assert(objAddedToAtLeastOne);//every object should get added to at least the left or the right child
    }

    objList.clear();

    pNode->setLeftChild(pLeft);
    pNode->setRightChild(pRight);

    if(_splitList.size() > 0)
    {
        //stick left over splits back into pool
        _splitPool.splice(_splitPool.end(), 
                          _splitList, _splitList.begin(), _splitList.end());
    }

    if (leftCount > 2) 
        subdivide(pLeft, bboxBestLeft, depth + 1, leftCount);
    
    if (rightCount > 2) 
        subdivide(pRight, bboxBestRight, depth + 1, rightCount);
}

void KDTree::computeBestSplitPlanePos(float& lowestCost, float& bestPos,
                                      BoundingBox& bboxLeftBest, BoundingBox& bboxRightBest,
                                      float minPos,
                                      float maxPos,
                                      BoundingBox& bboxLeft,
                                      BoundingBox& bboxRight,
                                      const SplitPosList& splitList,
                                      float curNodeSurfArea,
                                      KDTreeNode::SplitAxis axis)
{
    lowestCost = FLT_MAX;
    bestPos = 0.0f;
    for(SplitPosList::const_iterator itr = splitList.begin();
        itr != _splitList.end();
        ++itr)
    {
        const SplitPos& splitPos = *itr;
        // compute child node extents
        bboxRight.getCorner()[(int)axis] = splitPos.pos;
        bboxRight.getSizeVec()[(int)axis] = maxPos - splitPos.pos;
        bboxLeft.getSizeVec()[(int)axis] = splitPos.pos - minPos;

        // compute child node cost
        float leftChildCost = 2.0f * 
                    (bboxLeft.width() * bboxLeft.depth() + 
                     bboxLeft.width() * bboxLeft.height() + 
                     bboxLeft.depth() * bboxLeft.height());

        float rightChildCost = 2.0f * 
                    (bboxRight.width() * bboxRight.depth() + 
                     bboxRight.width() * bboxRight.height() + 
                     bboxRight.depth() * bboxRight.height());

        float splitcost = 0.3f + 1.0f * 
                          (leftChildCost * curNodeSurfArea * splitPos.leftCount + 
                          rightChildCost * curNodeSurfArea * splitPos.rightCount);

        // update best cost tracking variables
        if (splitcost < lowestCost)
        {
            lowestCost = splitcost;
            bestPos = splitPos.pos;
            bboxLeftBest = bboxLeft;
            bboxRightBest = bboxRight;
        }
    }
}

void KDTree::buildSplitPosCandidates(const KDTreeNode::ObjectList& objList,
                                     KDTreeNode::SplitAxis axis,
                                     float minPos,
                                     float maxPos,
                                     std::vector<float>& objMinPositions,
                                     std::vector<float>& objMaxPositions)
{
    size_t index = 0;
    for(KDTreeNode::ObjectList::const_iterator itr = objList.begin();
        itr != objList.end();
        ++itr)
    {
        const Node* pObj = *itr;

        const BoundingBox& bbox = pObj->boundingBox();

        float objMin = objMinPositions[index] = bbox.getCorner()[(int)axis];
        float objMax = objMaxPositions[index] = bbox.getFarCorner()[(int)axis];

        ++index;

        if(objMin >= minPos && objMin <= maxPos)
            insertSplitPos(objMin);
        if(objMax >= minPos && objMax <= maxPos)
            insertSplitPos(objMax);
    }    
}

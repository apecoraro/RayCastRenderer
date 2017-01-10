#pragma once

#include "Referenced.h"
#include "SmartPtr.h"
#include "Node.h"
#include "Group.h"
#include "Ray.h"
#include "Plane.h"
#include "Vec3.h"

#include <list>

namespace cs500
{
    class KDTreeNode : public Referenced
    {
    public:
	    KDTreeNode();
        
        bool intersect(const Ray& ray, Node::IntersectionData& data) const;
        enum SplitAxis
        {
            X_AXIS=0,
            Y_AXIS=1,
            Z_AXIS=2
        };

        void setSplitPlane(SplitAxis axis, float splitPos);
        SplitAxis getSplitAxis() const { return _axis; }
	    const Vec3& getSplitAxisVec() const { _splitPlane.normal(); }

	    void setLeftChild(KDTreeNode* pLeftChild) { _spLeftChild = pLeftChild; }
	    KDTreeNode* getLeftChild() { return _spLeftChild; }
        const KDTreeNode* getLeftChild() const { return _spLeftChild; }

        void setRightChild(KDTreeNode* pRightChild) { _spRightChild = pRightChild; }
	    KDTreeNode* getRightChild() { return _spRightChild; }
        const KDTreeNode* getRightChild() const { return _spRightChild; }
	    
	    bool isLeaf() const { return (_spLeftChild.get() == NULL && _spRightChild.get() == NULL); }

        typedef std::vector< SmartPtr<Node> > ObjectList;

	    const ObjectList& getObjects() const { return _objectList; }
        ObjectList& getObjects() { return _objectList; }
	    
	    void setObjects(ObjectList& list) { _objectList = list; }
        
        void addObject(Node* pObj) { _objectList.push_back(pObj); }
        size_t numObjects() { return _objectList.size(); }
    private:
        Plane _splitPlane;
        SplitAxis _axis;
        SmartPtr<KDTreeNode> _spLeftChild;
        SmartPtr<KDTreeNode> _spRightChild;
        ObjectList _objectList;
    };


    class KDTree : public Node
    {
    public:
        KDTree();
        void build(Group* pScene);

        bool intersect(const Ray& ray, 
                       Node::IntersectionData& data) const;

        virtual BoundingSphere computeBoundingSphere() const;
        virtual BoundingBox computeBoundingBox() const;

    private:
        float computeCostAndBestSplitPosOfAxis(KDTreeNode::SplitAxis axis, 
                                              const KDTreeNode::ObjectList& objList, 
                                              const BoundingBox& bbox,
                                              size_t numObjs,
                                              float& bestPos,
                                              BoundingBox& bboxBestLeft,//bounding box of left child with best split
                                              BoundingBox& bboxBestRight,//bounding box of right child with best split)
                                              std::vector<float>& objMinPositions,
                                              std::vector<float>& objMaxPositions);

        void insertSplitPos(float splitPos);
	    void subdivide(KDTreeNode* pNode, const BoundingBox& BBox, 
                       size_t depth, size_t numObjs);

        void buildSplitPosCandidates(const KDTreeNode::ObjectList& objList,
                                     KDTreeNode::SplitAxis axis,
                                     float minPos,
                                     float maxPos,
                                     std::vector<float>& rangeLeft,
                                     std::vector<float>& rangeRight);

        struct SplitPos
        {
            int leftCount;
            int rightCount;
            float pos;
            SplitPos() : leftCount(0), rightCount(0), pos(0.0f) {}
        };

        typedef std::list<SplitPos> SplitPosList;
        
        void computeBestSplitPlanePos(float& lowestCost, float& bestPos,
                                      BoundingBox& b1, BoundingBox& b2,
                                      float minPos,
                                      float maxPos,
                                      BoundingBox& b3,
                                      BoundingBox& b4,
                                      const SplitPosList& splitList,
                                      float curNodeSurfArea,
                                      KDTreeNode::SplitAxis axis);

	    SmartPtr<KDTreeNode> _spRoot;

        SplitPosList _splitList;
        SplitPosList _splitPool;
    };
};

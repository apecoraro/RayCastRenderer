#pragma once

#include "Node.h"
#include "Box.h"
#include "Camera.h"
#include "Referenced.h"
#include "Ray.h"
#include "Polygon.h"
#include "Vec3.h"
#include "Vec2.h"

#include <vector>

namespace cs500
{
    class HeightField : public Node
    {
    public:
        //2D vector of floats [row][col] or [y][x]
        typedef std::vector< std::vector<float> > GridPoints;
        typedef std::vector< std::vector<Vec3> > GridNormals;
        typedef std::vector< std::vector<Vec2> > GridTexCoords;

        HeightField(const GridPoints& gridPoints,
                    const GridTexCoords& texCoords,
                    float originX,
                    float originY,
                    float gridSpacingX,
                    float gridSpacingY,
                    float lodScalar=1.0f);

        virtual bool intersect(const Ray& ray, 
                               IntersectionData& data) const;

        virtual BoundingSphere computeBoundingSphere() const;
        virtual BoundingBox computeBoundingBox() const;

        virtual bool hasThickness() const { return false; }
        virtual bool useSpatialTree() const { return false; }

        void showLODs(bool flag) { _showLODs = flag; }

        virtual void computeProjectedSize(const Camera& camera, int horizRes);

        struct GridIndex
        {
            GridIndex(size_t y, size_t x) : y_row(y), x_col(x) {}
            size_t y_row;
            size_t x_col;
        };

        Vec3 getPoint(size_t row, size_t col);
    private:
        void computeGridNormals();
        void buildOctTrees();

        GridPoints _gridPoints;
        GridNormals _gridNormals;
        GridTexCoords _gridTexCoords;

        //minimum x and y
        float _originX;
        float _originY;
        float _gridSpacingX;
        float _gridSpacingY;
        float _lodScalar;
        bool _showLODs;

        class QuadTreeNode : public Referenced
        {
        public:
            
            QuadTreeNode(const GridPoints& gridPoints,
                         const GridNormals& gridNormals,
                         const GridTexCoords& texCoords,
                         float nodeOriginX,
                         float nodeOriginY,
                         size_t nodeWidth,
                         size_t nodeLength,
                         float gridSpacingX,
                         float gridSpacingY,
                         const GridIndex& indexLL,
                         const GridIndex& indexLR,
                         const GridIndex& indexUL,
                         const GridIndex& indexUR,
                         float lodScalar);

            bool intersect(const Ray& ray, 
                           IntersectionData& data) const;

            void computeMaxPoints(float& maxX, float& maxY, float& zMin, float& zMax) const;
            void computeProjectedSize(const Camera& camera, int horizRes);
        private:
            void createChildNodes();
            void createIntersectionPolygon();
            void createIntersectionBox();
            bool intersectWithChildNodes(const Ray& ray, IntersectionData& data) const;

            typedef std::vector< SmartPtr<QuadTreeNode> > ChildNodes;

            ChildNodes _children;
            const GridIndex _indexLL;
            const GridIndex _indexLR;
            const GridIndex _indexUL;
            const GridIndex _indexUR;

            const GridPoints& _gridPoints;
            const GridNormals& _gridNormals;
            const GridTexCoords& _gridTexCoords;
            //minimum x and y
            float _nodeOriginX;
            float _nodeOriginY;
            size_t _nodeWidth; //this node's x dimension
            size_t _nodeLength; //this node's y dimension
            float _gridSpacingX;
            float _gridSpacingY;

            SmartPtr<Box> _spBoundingBox;//for broad base intersection test
            SmartPtr<Polygon> _spPolygon;//for fine grained intersection
            SmartPtr<Polygon> _spPolygon2;

            float _projSize;
            float _pixelSize;
            float _lodScalar;
        };

        SmartPtr<QuadTreeNode> _spTreeRoot;
    };
};
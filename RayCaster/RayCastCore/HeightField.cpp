#include "HeightField.h"

#include "Triangle.h"

#include <float.h>

using namespace cs500;

HeightField::HeightField(const GridPoints& gridPoints,
                         const GridTexCoords& texCoords,
                         float originX,
                         float originY,
                         float gridSpacingX,
                         float gridSpacingY,
                         float lodScalar/*=1.0f*/) :
    _gridPoints(gridPoints),
    _gridTexCoords(texCoords),
    _originX(originX),
    _originY(originY),
    _gridSpacingX(gridSpacingX),
    _gridSpacingY(gridSpacingY),
    _lodScalar(lodScalar),
    _showLODs(false)
{
    computeGridNormals();
    buildOctTrees();
}

Vec3 HeightField::getPoint(size_t row, size_t col)
{
    float x = _originX + (_gridSpacingX * col);
    float y = _originY + (_gridSpacingY * row);
    float z = _gridPoints.at(row).at(col);

    return Vec3(x, y, z);
}

void HeightField::computeGridNormals()
{
    for(size_t row = 0; row < _gridPoints.size(); ++row)
    {
        std::vector<Vec3> rowNormals;
        for(size_t col = 0; col < _gridPoints.at(0).size(); ++col)
        {
            Vec3 rowCol = getPoint(row, col);

            Vec3 rowColPlus1;
            Vec3 rowPlus1ColPlus1;
            bool neTri1 = false;
            bool neTri2 = false;
            
            if(col+1 < _gridPoints.at(0).size())
            {
                rowColPlus1 = getPoint(row, col+1);
                if(row+1 < _gridPoints.size())
                {
                    neTri1 = true;
                    neTri2 = true;
                    rowPlus1ColPlus1 = getPoint(row+1, col+1);
                }
            }

            
            Vec3 rowPlus1Col;
            bool nwTri = false;
            if(row+1 < _gridPoints.size())
            {
                nwTri = true;
                rowPlus1Col = getPoint(row+1, col);
            }

            Vec3 rowColMinus1;
            Vec3 rowMinus1ColMinus1;
            bool swTri1 = false;
            bool swTri2 = false;
            if(col != 0)//col-1>=0
            {
                rowColMinus1 = getPoint(row, col-1);
                if(row != 0)
                {
                    swTri1 = true;
                    swTri2 = true;
                    rowMinus1ColMinus1 = getPoint(row-1, col-1);
                }
            }
            else
                nwTri = false;

            Vec3 rowMinus1Col;
            bool seTri = false;
            if(row != 0)//row-1>=0
            {
                rowMinus1Col = getPoint(row-1, col);
                if(col+1 < _gridPoints.at(0).size())
                    seTri = true;
            }

            Vec3 avgNorm;

            if(neTri1)
            {
                Triangle neTriangle1(rowCol, rowColPlus1, rowPlus1ColPlus1);
                avgNorm = Vec3::normalize(neTriangle1.normal());
            }

            if(neTri2)
            {
                Triangle neTriangle2(rowCol, rowPlus1ColPlus1, rowPlus1Col);
                avgNorm += Vec3::normalize(neTriangle2.normal());
            }

            if(nwTri)
            {
                Triangle nwTriangle(rowCol, rowPlus1Col, rowColMinus1);
                avgNorm += Vec3::normalize(nwTriangle.normal());
                /*if(neTri)
                    avgNorm *= 0.5f;*/
            }

            if(swTri1)
            {
                Triangle swTriangle1(rowCol, rowColMinus1, rowMinus1ColMinus1);
                avgNorm += Vec3::normalize(swTriangle1.normal());
                /*if(nwTri || neTri)
                    avgNorm *= 0.5f;*/
            }

            if(swTri2)
            {
                Triangle swTriangle2(rowCol, rowMinus1ColMinus1, rowMinus1Col);
                avgNorm += Vec3::normalize(swTriangle2.normal());
                /*if(nwTri || neTri)
                    avgNorm *= 0.5f;*/
            }

            if(seTri)
            {
                Triangle seTriangle(rowCol, rowMinus1Col, rowColPlus1);
                avgNorm += Vec3::normalize(seTriangle.normal());
                /*if(nwTri || neTri || swTri)
                    avgNorm *= 0.5f;*/
            }

            avgNorm.normalize();
            rowNormals.push_back(avgNorm);
        }

        _gridNormals.push_back(rowNormals);
    }
}

void HeightField::buildOctTrees()
{
    //create root node
    GridIndex llIndex(0, 0);
    GridIndex lrIndex(0, _gridPoints.at(0).size()-1);
    GridIndex ulIndex(_gridPoints.size()-1, 0);
    GridIndex urIndex(_gridPoints.size()-1, _gridPoints.at(0).size()-1);

    _spTreeRoot = new QuadTreeNode(_gridPoints, 
                                   _gridNormals,
                                   _gridTexCoords,
                                   _originX, _originY, 
                                   _gridPoints.at(0).size(), _gridPoints.size(),
                                   _gridSpacingX, _gridSpacingY,
                                   llIndex, lrIndex, ulIndex, urIndex,
                                   _lodScalar);

}

void HeightField::computeProjectedSize(const Camera& camera, int horizRes)
{
    _spTreeRoot->computeProjectedSize(camera, horizRes);
}

bool HeightField::intersect(const Ray& ray, 
                            IntersectionData& data) const
{
    if(_spTreeRoot->intersect(ray, data))
    {
        if(!_showLODs)
            data.spIntersectedObject = this;
        return true;
    }

    return false;
}

BoundingSphere HeightField::computeBoundingSphere() const
{
    float maxX, maxY, maxZ, minZ;
    _spTreeRoot->computeMaxPoints(maxX, maxY, minZ, maxZ);

    Vec3 ll(_originX, _originY, minZ);
    Vec3 ur(maxX, maxY, maxZ);

    Vec3 center = ll;
    Vec3 diffToCenter((maxX - _originX)*0.5f, (maxY - _originY)*0.5f, (maxZ - minZ)*0.5f);
    center += diffToCenter;

    BoundingSphere sphere(center, (ll - center).length());

    return sphere;
}

BoundingBox HeightField::computeBoundingBox() const
{
    float maxX, maxY, maxZ, minZ;
    _spTreeRoot->computeMaxPoints(maxX, maxY, minZ, maxZ);

    Vec3 ll(_originX, _originY, minZ);
    Vec3 ur(maxX, maxY, maxZ);

    BoundingBox bounds;
    bounds.expandBy(ll);
    bounds.expandBy(ur);

    return bounds;
}

HeightField::QuadTreeNode::QuadTreeNode(const GridPoints& gridPoints,
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
                                        float lodScalar) :
    _gridPoints(gridPoints),
    _gridNormals(gridNormals),
    _gridTexCoords(texCoords),
    _nodeOriginX(nodeOriginX),
    _nodeOriginY(nodeOriginY),
    _nodeWidth(nodeWidth), //x axis dimension
    _nodeLength(nodeLength), //y axis dimension
    _gridSpacingX(gridSpacingX),
    _gridSpacingY(gridSpacingY),
    _indexLL(indexLL),
    _indexLR(indexLR),
    _indexUL(indexUL),
    _indexUR(indexUR),
    _lodScalar(lodScalar)
{
    createChildNodes();

    createIntersectionBox();

    createIntersectionPolygon();
}

void HeightField::QuadTreeNode::createIntersectionBox()
{
    float minZ, maxZ, maxX, maxY;
    computeMaxPoints(maxX, maxY, minZ, maxZ);

    Vec3 corner(_nodeOriginX, _nodeOriginY, minZ);
    Vec3 xVec(maxX - _nodeOriginX, 0.0f, 0.0f);
    Vec3 yVec(0.0f, maxY - _nodeOriginY, 0.0f);
    Vec3 zVec(0.0f, 0.0f, maxZ - minZ);

    _spBoundingBox = new Box(corner, xVec, yVec, zVec);
}

void HeightField::QuadTreeNode::computeMaxPoints(float& maxX, 
                                                 float& maxY, 
                                                 float& zMin, 
                                                 float& zMax) const
{
    float minZ = FLT_MAX;
    float maxZ = -FLT_MAX;
    for(size_t rowIndex = _indexLL.y_row; rowIndex <= _indexUR.y_row; ++rowIndex)
    {
        for(size_t colIndex = _indexLL.x_col; colIndex <= _indexUR.x_col; ++colIndex)
        {
            float z = _gridPoints.at(rowIndex).at(colIndex);
            
            if(z < minZ)
                minZ = z;
            
            if(z > maxZ)
                maxZ = z;
        }
    }

    zMin = minZ;
    zMax = maxZ;

    maxX = _nodeOriginX + (_gridSpacingX * (_nodeWidth-1));
    maxY = _nodeOriginY + (_gridSpacingY * (_nodeLength-1));
}

void HeightField::QuadTreeNode::computeProjectedSize(const Camera& camera, int horizRes)
{
    Vec3 lenVec = _spBoundingBox->lengthVector() 
                    + _spBoundingBox->widthVector() 
                    + _spBoundingBox->heightVector();
    
    float radius = lenVec.length() * 0.5f;

    Vec3 midPoint = _spBoundingBox->center();

    Vec3 camVAxis = Vec3::normalize(camera.vAxis());
    Vec3 camUAxis = Vec3::normalize(camera.uAxis());

    Vec3 viewPlaneLL = midPoint - (camVAxis * radius) - (camUAxis * radius);
    Vec3 camToLL(viewPlaneLL - camera.eyePos());
    camToLL.normalize();

    Vec3 viewPlaneUR = midPoint + (camVAxis * radius) + (camUAxis * radius);
    Vec3 camToUR(viewPlaneUR - camera.eyePos());
    camToUR.normalize();

    _projSize = 1.0f - Vec3::dot(camToLL, camToUR);

    _pixelSize = camera.pixelSize(horizRes);

    for(size_t i = 0; i < _children.size(); ++i)
    {
        _children.at(i)->computeProjectedSize(camera, horizRes);
    }
}

bool HeightField::QuadTreeNode::intersect(const Ray& ray, 
                                          IntersectionData& data) const
{
    
    IntersectionData boxIsectData;
    //if _nodeWidth + _nodeLength < 6 then there are only 
    //4 or fewer triangles to do intersection test
    //with so don't bother intersecting with the bounding box
    if(_nodeWidth + _nodeLength < 6 || _spBoundingBox->intersect(ray, boxIsectData))
    {
        if(_children.size() == 0 || _projSize < (_pixelSize * _lodScalar))
        {
            //if i have no children or 
            //if my projected size is smaller than the (pixel  size * lod scalar)
            //then just use my grid points/triangles
            return _spPolygon->intersect(ray, data);
            //debugging code
            //IntersectionData data1, data2;
            //bool t1 = _spPolygon->intersect(ray, data1);
            //bool t2 = _spPolygon2->intersect(ray, data2);

            //if(t1 && t2)
            //{
            //    if(data1.isectTVal < data2.isectTVal)
            //        data = data1;
            //    else
            //        data = data2;
            //}
            //else if(t1)
            //    data = data1;
            //else if(t2)
            //    data = data2;
        
            //return t1 || t2;
        }
        else
        {
            //otherwise intersect the children
            return intersectWithChildNodes(ray, data);
        }
    }

    return false;
}

bool HeightField::QuadTreeNode::intersectWithChildNodes(const Ray& ray, IntersectionData& data) const
{
    bool success = false;
    for(size_t i = 0; i < _children.size(); ++i)
    {
        IntersectionData curData;
        if(_children.at(i)->intersect(ray, curData))
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

void HeightField::QuadTreeNode::createIntersectionPolygon()
{
    _spPolygon = new Polygon();
    //this polygon's surface properties are only used if debugging
    _spPolygon->setSurfaceProperties(new SurfaceProperties(Vec3(1.0f - 2.0f/static_cast<float>(_nodeLength),
                                                                1.0f - 2.0f/static_cast<float>(_nodeWidth),
                                                                1.0f),
                                                            0.2f,
                                                            10.0f,
                                                            Vec3(),
                                                            1000000.0f,
                                                            1.0f));
    //_spPolygon2 = new Polygon();
    //_spPolygon2->setSurfaceProperties(new SurfaceProperties(Vec3(1,0,0),
    //                                                        0.2,
    //                                                        10.0,
    //                                                        Vec3(0,0,0),
    //                                                        1000000.0,
    //                                                        1.0));

    float minX = _nodeOriginX;
    float minY = _nodeOriginY;
    float maxX = _nodeOriginX + (_gridSpacingX * (_nodeWidth-1));
    float maxY = _nodeOriginY + (_gridSpacingY * (_nodeLength-1));
    
    //ll
    float z = _gridPoints.at(_indexLL.y_row).at(_indexLL.x_col);
    Vec3 p0(minX, minY, z);
    const Vec3& n0 = _gridNormals.at(_indexLL.y_row).at(_indexLL.x_col);
    const Vec2& uv0 = _gridTexCoords.at(_indexLL.y_row).at(_indexLL.x_col);
    
    //lr
    z = _gridPoints.at(_indexLR.y_row).at(_indexLR.x_col);
    Vec3 p1(maxX, minY, z);
    const Vec3& n1 = _gridNormals.at(_indexLR.y_row).at(_indexLR.x_col);
    const Vec2& uv1 = _gridTexCoords.at(_indexLR.y_row).at(_indexLR.x_col);


    //ur
    z = _gridPoints.at(_indexUR.y_row).at(_indexUR.x_col);
    Vec3 p2(maxX, maxY, z);
    const Vec3& n2 = _gridNormals.at(_indexUR.y_row).at(_indexUR.x_col);
    const Vec2& uv2 = _gridTexCoords.at(_indexUR.y_row).at(_indexUR.x_col);

    //ul
    z = _gridPoints.at(_indexUL.y_row).at(_indexUL.x_col);
    Vec3 p3(minX, maxY, z);
    const Vec3& n3 = _gridNormals.at(_indexUL.y_row).at(_indexUL.x_col);
    const Vec2& uv3 = _gridTexCoords.at(_indexUL.y_row).at(_indexUL.x_col);

    Triangle tri0(p0, p1, p2);
    tri0.setNormals(n0, n1, n2);
    tri0.setTexCoords(uv0, uv1, uv2);

    _spPolygon->addTriangle(tri0);

    Triangle tri1(p0, p2, p3);
    tri1.setNormals(n0, n2, n3);
    tri1.setTexCoords(uv0, uv2, uv3);

    //_spPolygon2->addTriangle(tri1);
    _spPolygon->addTriangle(tri1);
}

void HeightField::QuadTreeNode::createChildNodes()
{
    size_t childNodeIndexWidth = _nodeWidth >> 1;//1
    size_t childNodeIndexLength = _nodeLength >> 1;//2
    
    if(_nodeWidth > 2 || _nodeLength > 2)
    {
        {
            float childOriginX = _nodeOriginX;
            float childOriginY = _nodeOriginY;

            //sw node indices
            GridIndex llIndex = _indexLL;
            GridIndex lrIndex(_indexLL.y_row, _indexLL.x_col + childNodeIndexWidth);

            GridIndex ulIndex(_indexLL.y_row + childNodeIndexLength, _indexLL.x_col);
            GridIndex urIndex(ulIndex.y_row, lrIndex.x_col);


            size_t childNodeWidth = lrIndex.x_col - llIndex.x_col + 1;
            size_t childNodeLength = ulIndex.y_row - llIndex.y_row + 1;
            
            QuadTreeNode* pNode = new QuadTreeNode(_gridPoints,
                                                    _gridNormals,
                                                    _gridTexCoords,
                                                   childOriginX,
                                                   childOriginY,
                                                   childNodeWidth,
                                                   childNodeLength,
                                                   _gridSpacingX,
                                                   _gridSpacingY,
                                                   llIndex, lrIndex, ulIndex, urIndex,
                                                   _lodScalar);
            _children.push_back(pNode);
        }

        if(_nodeWidth > 2)
        {
            float childOriginX = _nodeOriginX + (_gridSpacingX * static_cast<float>(childNodeIndexWidth));
            float childOriginY = _nodeOriginY;
                                          
            //se node indices
            GridIndex llIndex(_indexLL.y_row, _indexLL.x_col + childNodeIndexWidth);
            GridIndex lrIndex(_indexLL.y_row, _indexLL.x_col + _nodeWidth - 1);

            GridIndex ulIndex(_indexLL.y_row + childNodeIndexLength, llIndex.x_col);
            GridIndex urIndex(ulIndex.y_row, lrIndex.x_col);

            size_t childNodeWidth = lrIndex.x_col - llIndex.x_col + 1;
            size_t childNodeLength = ulIndex.y_row - llIndex.y_row + 1;
            
            QuadTreeNode* pNode = new QuadTreeNode(_gridPoints,
                                                    _gridNormals,
                                                    _gridTexCoords,
                                                     childOriginX,
                                                     childOriginY,
                                                     childNodeWidth,
                                                     childNodeLength,
                                                     _gridSpacingX,
                                                     _gridSpacingY,
                                                     llIndex, lrIndex, ulIndex, urIndex,
                                                     _lodScalar);

            _children.push_back(pNode);
        }
        
        if(_nodeLength > 2)
        {
            float childOriginX = _nodeOriginX;
            float childOriginY = _nodeOriginY + (_gridSpacingY * static_cast<float>(childNodeIndexLength));

            //nw node indices
            GridIndex llIndex(_indexLL.y_row + childNodeIndexLength, _indexLL.x_col);
            GridIndex lrIndex(llIndex.y_row, _indexLL.x_col + childNodeIndexWidth);

            GridIndex ulIndex(_indexLL.y_row + _nodeLength - 1, _indexLL.x_col);
            GridIndex urIndex(ulIndex.y_row, lrIndex.x_col);


            size_t childNodeWidth = lrIndex.x_col - llIndex.x_col + 1;
            size_t childNodeLength = ulIndex.y_row - llIndex.y_row + 1;
            
            QuadTreeNode* pNode = new QuadTreeNode(_gridPoints,
                                                    _gridNormals,
                                                    _gridTexCoords,
                                                   childOriginX,
                                                   childOriginY,
                                                   childNodeWidth,
                                                   childNodeLength,
                                                   _gridSpacingX,
                                                   _gridSpacingY,
                                                   llIndex, lrIndex, ulIndex, urIndex,
                                                   _lodScalar);
            _children.push_back(pNode);
        }

        if(_nodeWidth > 2 && _nodeLength > 2)
        {
            float childOriginX = _nodeOriginX + (_gridSpacingX * static_cast<float>(childNodeIndexWidth));
            float childOriginY = _nodeOriginY + (_gridSpacingY * static_cast<float>(childNodeIndexLength));
                                          
            //ne node indices
            GridIndex llIndex(_indexLL.y_row + childNodeIndexLength, _indexLL.x_col + childNodeIndexWidth);
            GridIndex lrIndex(llIndex.y_row, _indexLL.x_col + _nodeWidth - 1);

            GridIndex ulIndex(_indexLL.y_row + _nodeLength - 1, llIndex.x_col);
            GridIndex urIndex(ulIndex.y_row, lrIndex.x_col);


            size_t childNodeWidth = lrIndex.x_col - llIndex.x_col + 1;
            size_t childNodeLength = ulIndex.y_row - llIndex.y_row + 1;
            
            QuadTreeNode* pNode = new QuadTreeNode(_gridPoints,
                                                    _gridNormals,
                                                    _gridTexCoords,
                                                   childOriginX,
                                                   childOriginY,
                                                   childNodeWidth,
                                                   childNodeLength,
                                                   _gridSpacingX,
                                                   _gridSpacingY,
                                                   llIndex, lrIndex, ulIndex, urIndex,
                                                   _lodScalar);
            _children.push_back(pNode);
        }
    }
}
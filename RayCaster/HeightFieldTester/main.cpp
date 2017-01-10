#include "RayCaster.h"
#include "SmartPtr.h"

#include "Sphere.h"
#include "Box.h"
#include "Matrix.h"
#include "Ellipsoid.h"
#include "Ray.h"
#include "Polygon.h"
#include "Image.h"
#include "Timer.h"
#include "Sampler.h"
#include "UniformGridSampler.h"
#include "JitteredGridSampler.h"
#include "HeightField.h"
#include "Camera.h"
#include "Ray.h"
#include "Node.h"

#include <iostream>
#include <sstream>
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace cs500;

int main(int argc, char* argv[])
{
    HeightField::GridPoints gridPoints;
    std::vector<float> row;
    row.push_back(0.0f);
    row.push_back(0.0f);

    gridPoints.push_back(row);

    row.clear();
    row.push_back(5.0f);
    row.push_back(5.0f);
    
    gridPoints.push_back(row);

    HeightField::GridTexCoords gridUVs;
    std::vector<Vec2> uvRow;

    uvRow.push_back(Vec2(0.0f, 0.0f));
    uvRow.push_back(Vec2(0.0f, 1.0f));

    gridUVs.push_back(uvRow);

    uvRow.clear();
    uvRow.push_back(Vec2(1.0f, 0.0f));
    uvRow.push_back(Vec2(1.0f, 1.0f));

    gridUVs.push_back(uvRow);

    SmartPtr<HeightField> spHF = new HeightField(gridPoints, gridUVs, 0.0f, 0.0f, 10.0f, 10.0f);

    Vec3 vpCenter(2.5, -1.0f, 5.0f);
    cs500::Matrix rot(1.0f,               0.0f,              0.0f, 0.0f,
                      0.0f,  cosf(0.34906585f), sinf(0.34906585f), 0.0f,
                      0.0f, -sinf(0.34906585f), cosf(0.34906585f), 0.0f,
                      0.0f,               0.0f,              0.0f, 1.0f);

    Vec3 uVec(0.7f, 0.0f, 0.0f);
    uVec = rot.transformVector(uVec);

    Vec3 vVec(0.0f, 0.0f, 0.7f);
    vVec = rot.transformVector(vVec);

    Vec3 eyeVec(0.0f, -1.0f, 0.0f);
    eyeVec = rot.transformVector(eyeVec);
    eyeVec.normalize();

    SmartPtr<Camera> spCamera = new Camera(vpCenter, uVec, vVec, eyeVec);

    //hf.computeProjectedSize(*spCamera.get(), 400);
    Vec3 rayDir = Vec3::normalize(spCamera->center() - spCamera->eyePos());
    Ray ray(spCamera->eyePos(), rayDir);

    Node::IntersectionData data;
    spHF->intersect(ray, data);

    return 0;

} 
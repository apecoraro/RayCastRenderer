#include "RayCaster.h"
#include "SceneFactory.h"

#include "Box.h"
#include "Sphere.h"
#include "Polygon.h"
#include "Ellipsoid.h"
#include "SurfaceProperties.h"
#include "Random.h"
#include "HeightFieldBuilder.h"
#include "Vec3.h"
#include "Vec2.h"

#include <string>
#include <sstream>
#include <iostream>

using namespace cs500;

static const double TwoPI = 6.28318530717958647692;

static float k_precEpsilon = 0.0005f;

static bool ParseFloat(std::stringstream& str, float& val)
{
    str >> val;

    return !str.fail();
}

static bool ParseFloat(const std::string floatStr, float& val)
{
    std::stringstream str;
    str << floatStr;

    return ParseFloat(str, val);
}

static size_t FindChar(char c, 
                       std::string& vecStr, 
                       std::stringstream& lineStream, 
                       size_t& startIndex)
{
    size_t index = std::string::npos;
    do
    {
        index = vecStr.find(c, startIndex);
        if(index == std::string::npos)
        {
            lineStream >> vecStr;
            startIndex = 0;
        }
        else
            break;
    }
    while(!lineStream.eof());

    return index;
}

static bool ParseVec2(std::stringstream& lineStream, Vec2& vec)
{
    std::string vecStr;
    lineStream >> vecStr;

    if(lineStream.fail())
        return false;

    size_t start = 0;
    start = FindChar('(', vecStr, lineStream, start) + 1;
    
    size_t end = FindChar(',', vecStr, lineStream, start);

    if(!ParseFloat(vecStr.substr(start, end-start), vec.x()))
        return false;

    start = end+1;
    end = FindChar(')', vecStr, lineStream, start);

    if(!ParseFloat(vecStr.substr(start, end-start), vec.y()))
        return false;

    return true;
}

static bool ParseVec3(std::stringstream& lineStream, Vec3& vec)
{
    std::string vecStr;
    lineStream >> vecStr;

    if(lineStream.fail())
        return false;

    size_t start = 0;
    start = FindChar('(', vecStr, lineStream, start) + 1;
    
    size_t end = FindChar(',', vecStr, lineStream, start);

    if(!ParseFloat(vecStr.substr(start, end-start), vec.x()))
        return false;

    start = end+1;
    end = FindChar(',', vecStr, lineStream, start);

    if(!ParseFloat(vecStr.substr(start, end-start), vec.y()))
        return false;

    start = end+1;
    end = FindChar(')', vecStr, lineStream, start);

    if(!ParseFloat(vecStr.substr(start, end-start), vec.z()))
        return false;

    return true;
}

static bool ParseString(std::stringstream& lineStream, std::string& str)
{
    
    lineStream >> str;

    if(lineStream.fail())
        return false;

    size_t quoteStart = 0;
    quoteStart = str.find('"');
    if(quoteStart != std::string::npos)
    {
        str = str.substr(1);
        while(!lineStream.eof())
        {

            std::string next;
            lineStream >> next;

            str += " ";
            str += next;

            size_t end = next.find('"');
            if(end != std::string::npos)
            {
                str = str.substr(0, str.length()-1);
                return true;
            }
        }        
    }

    return true;
}

static bool ParseSphere(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    Vec3 centerVec;
    if(!ParseVec3(lineStream, centerVec))
        return NULL;

    float radius;
    lineStream >> radius;

    if(lineStream.fail())
        return NULL;

    pRayCaster->addSceneObject(new Sphere(centerVec, radius));

    return true;
}

static bool ParseBox(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    Vec3 corner;
    if(!ParseVec3(lineStream, corner))
        return NULL;
    Vec3 lenVec;
    if(!ParseVec3(lineStream, lenVec))
        return NULL;
    Vec3 widVec;
    if(!ParseVec3(lineStream, widVec))
        return NULL;
    Vec3 hgtVec;
    if(!ParseVec3(lineStream, hgtVec))
        return NULL;

    pRayCaster->addSceneObject(new Box(corner, lenVec, widVec, hgtVec));

    return true;
}

static bool ParseCamera(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    Vec3 center;
    if(!ParseVec3(lineStream, center))
        return NULL;
    Vec3 uVec;
    if(!ParseVec3(lineStream, uVec))
        return NULL;
    Vec3 vVec;
    if(!ParseVec3(lineStream, vVec))
        return NULL;
    Vec3 eyeVec;
    if(!ParseVec3(lineStream, eyeVec))
        return NULL;

    pRayCaster->setCamera(new Camera(center, uVec, vVec, eyeVec));

    return true;
}

static bool ParsePolygon(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    float numVerticesF;
    if(!ParseFloat(lineStream, numVerticesF))
        return false;
    size_t numVertices = static_cast<size_t>(numVerticesF);
    //parse the vertices of the polygon as a series of
    //triangle fan primitives
    Vec3 fulcrum;
    if(!ParseVec3(lineStream, fulcrum))
        return false;
        
    SmartPtr<Polygon> spPolygon = new Polygon();

    Vec3 pts[2];
    size_t ptsIndex = 0;
    size_t numParsedPoints = 0;
    while(!lineStream.eof() && numParsedPoints < numVertices-1)
    {
        ParseVec3(lineStream, pts[ptsIndex]);
        ++numParsedPoints;
        if(ptsIndex == 1)
        {
            spPolygon->addTriangle(fulcrum, pts[0], pts[1]);
            pts[0] = pts[1];
        }
        else
            ++ptsIndex;
    }

    if(spPolygon->getNumTriangles() == 0)
        return false;

    std::string textured;
    if(ParseString(lineStream, textured) && textured == "TEXTURED")
    {
        size_t numUVs = 0;
        std::vector<Vec2> uvs(numVertices);
        while(!lineStream.eof() && numUVs < numVertices)
        {
            if(!ParseVec2(lineStream, uvs[numUVs]))
                return false;
            ++numUVs;
        }

        if(numUVs != numVertices)
            return false;

        size_t uvIndex = 1;
        for(size_t tIndex = 0; tIndex < spPolygon->getNumTriangles(); ++tIndex)
        {
            Triangle& tri = spPolygon->getTriangle(tIndex);
            tri.setTexCoords(uvs[0], uvs[uvIndex], uvs[uvIndex+1]);
            ++uvIndex;
        }
    }

    pRayCaster->addSceneObject(spPolygon.get());
    return true;
}

static bool ParseEllipsoid(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    Vec3 center;
    if(!ParseVec3(lineStream, center))
        return false;

    Vec3 uAxis;
    if(!ParseVec3(lineStream, uAxis))
        return false;

    Vec3 vAxis;
    if(!ParseVec3(lineStream, vAxis))
        return false;

    Vec3 wAxis;
    if(!ParseVec3(lineStream, wAxis))
        return false;

    Ellipsoid* pEllipsoid = new Ellipsoid(center, uAxis, vAxis, wAxis);
    pRayCaster->addSceneObject(pEllipsoid);

    return true;
}

static bool ParseLight(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    Vec3 centerVec;
    if(!ParseVec3(lineStream, centerVec))
        return NULL;

    Vec3 colorVec;
    if(!ParseVec3(lineStream, colorVec))
        return NULL;

    float radius;
    lineStream >> radius;

    if(lineStream.fail())
        return NULL;

    pRayCaster->addLight(new Light(centerVec, colorVec, radius));

    return true;
}

static bool ParseAmbientLight(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    Vec3 color;
    if(!ParseVec3(lineStream, color))
        return false;

    pRayCaster->getEnvironment().setAmbientLight(color);
    return true;
}

static bool ParseAir(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    float relElecPerm;
    if(!ParseFloat(lineStream, relElecPerm))
        return false;

    float relMagPerm;
    if(!ParseFloat(lineStream, relMagPerm))
        return false;

    Vec3 attenuation;
    if(!ParseVec3(lineStream, attenuation))
        return false;

    pRayCaster->getEnvironment().setAir(*(new Air(relElecPerm, relMagPerm, attenuation)));
    return true;
}

static bool ParseHeightField(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    float xOrigin;
    if(!ParseFloat(lineStream, xOrigin))
        return false;

    float yOrigin;
    if(!ParseFloat(lineStream, yOrigin))
        return false;

    float gridSpacingX;
    if(!ParseFloat(lineStream, gridSpacingX))
        return false;

    float gridSpacingY;
    if(!ParseFloat(lineStream, gridSpacingY))
        return false;

    float zMax;
    if(!ParseFloat(lineStream, zMax))
        return false;

    std::string gridFile;
    if(!ParseString(lineStream, gridFile))
        return false;

    float lodScalar;
    if(!ParseFloat(lineStream, lodScalar))
        lodScalar = 1.0f;

    HeightField* pHeightField = 
        HeightFieldBuilder::CreateHeightField(gridFile, 
                                              zMax, 
                                              xOrigin, 
                                              yOrigin, 
                                              gridSpacingX, 
                                              gridSpacingY,
                                              lodScalar);
    
    if(pHeightField == NULL)
        return false;

    std::string debug;
    if(ParseString(lineStream, debug) && debug == "DEBUG")
        pHeightField->showLODs(true);
    
    pRayCaster->addSceneObject(pHeightField);
    return true;
}

static bool ParseSurfaceProperties(std::stringstream& lineStream, void* pUserData)
{
    RayCaster* pRayCaster = static_cast<RayCaster*>(pUserData);

    Vec3 diffuse;
    if(!ParseVec3(lineStream, diffuse))
        return false;

    float specularCoef;
    if(!ParseFloat(lineStream, specularCoef))
        return false;

    float specularExp;
    if(!ParseFloat(lineStream, specularExp))
        return false;

    Vec3 attenuationFactors;
    if(!ParseVec3(lineStream, attenuationFactors))
        return false;

    float relElecPerm;
    if(!ParseFloat(lineStream, relElecPerm))
        return false;

    float relMagPerm;
    if(!ParseFloat(lineStream, relMagPerm))
        return false;

    

    SurfaceProperties* pSurfProps = new SurfaceProperties(diffuse,
                                                          specularCoef,
                                                          specularExp,
                                                          attenuationFactors,
                                                          relElecPerm,
                                                          relMagPerm);

    std::string colorMap;
    if(ParseString(lineStream, colorMap) && colorMap == "COLORMAP")
    {
        std::string textureFile;
        if(ParseString(lineStream, textureFile))
        {
            Texture* pTexture = new Texture(textureFile);
            pSurfProps->setTexture(pTexture);
        }
    }

    pRayCaster->getSceneObject(pRayCaster->getNumSceneObjects()-1)->setSurfaceProperties(pSurfProps);
    return true;
}

bool RayCaster::loadScene(const std::string& inputFile)
{
    SceneFactory sceneFactory;

    sceneFactory.addObjectParser("SPHERE", ParseSphere);
    sceneFactory.addObjectParser("BOX", ParseBox);
    sceneFactory.addObjectParser("CAMERA", ParseCamera);
    sceneFactory.addObjectParser("POLYGON", ParsePolygon);
    sceneFactory.addObjectParser("ELLIPSOID", ParseEllipsoid);
    sceneFactory.addObjectParser("LIGHT", ParseLight);
    sceneFactory.addObjectParser("AMBIENT", ParseAmbientLight);
    sceneFactory.addObjectParser("AIR", ParseAir);
    sceneFactory.addObjectParser("HEIGHTFIELD", ParseHeightField);
    sceneFactory.setSurfacePropertiesParser(ParseSurfaceProperties);
    
    _spSceneObjects = new Group();

    if(!sceneFactory.parseSceneFile(inputFile, this))
    {
        std::cerr << "Failed to parse input file " << inputFile << std::endl;
        return false;
    }

    return true;
}

void RayCaster::buildSphereTree()
{
    if(_spSceneObjects.get() != NULL && _spSceneObjects->getNumChildren() > 0)
    {
        SphereTree* pSphereTree = new SphereTree();
        //this will remove children from the scene
        pSphereTree->build(_spSceneObjects.get());

        if(_spSceneObjects->getNumChildren() == 0)
            _spSceneObjects = pSphereTree;
        else
            _spSceneObjects->addChild(pSphereTree);
    }
}

void RayCaster::buildKDTree()
{
    if(_spSceneObjects.get() != NULL && _spSceneObjects->getNumChildren() > 0)
    {
        KDTree* pKDTree = new KDTree();
        //this will remove children from the scene
        pKDTree->build(_spSceneObjects.get());

        _spSceneObjects->addChild(pKDTree);
    }
}

static float ComputeReflectionCoef(float nIDivNt, 
                                   float mIDivMt, 
                                   float cosThetaI,
                                   float& cosThetaT)
{
    /*if(cosThetaI < 0.0f)
        cosThetaI = fabs(cosThetaI);
    else if(cosThetaI > 1.0f)
        cosThetaI = 1.0f;*/

    float cosSqThetaI = cosThetaI * cosThetaI;
    float oneMinusCosSqThetaI = 1.0f - cosSqThetaI;
    
    float radicand = 1 - ((nIDivNt * nIDivNt) * oneMinusCosSqThetaI);
    if(radicand < 0.0f)
        return 1.0f;

    //this value is used in computing the transmission vector 
    //(its the cos of the angle between the normal and the transmission vector)
    //so we'll return it so we don't have to recompute it
    cosThetaT = sqrtf(radicand);
    float rhsEPerp = mIDivMt * cosThetaT;
    float lhsEPerp = nIDivNt * cosThetaI;

    float topEPerp = lhsEPerp - rhsEPerp;
    float botEPerp = lhsEPerp + rhsEPerp;

    float ePerpREPerpI = topEPerp / botEPerp;

    float rhsEPll = nIDivNt * cosThetaT;
    float lhsEPll = mIDivMt * cosThetaI;

    float topEPll = lhsEPll - rhsEPll;
    float botEPll = lhsEPll + rhsEPll;

    float ePllREPllI = topEPll / botEPll;

    float reflCoef = 0.5f * ((ePerpREPerpI * ePerpREPerpI)  + (ePllREPllI * ePllREPllI));
    return reflCoef;
}

static void ComputeReflectionVector(const Vec3& vecToEye, 
                                    const Vec3& isectNorm,
                                    float normDotI,
                                    Vec3& reflVector)
{
    Vec3 lhs = isectNorm;
    float twoTimesIDotNorm = 2.0f * normDotI;
    lhs *= twoTimesIDotNorm;

    reflVector = lhs - vecToEye;
}

static void ComputeTransmissionVector(const Vec3& vecToEye, 
                                      const Vec3& isectNorm,
                                      float normDotI,
                                      float nIDivNt,
                                      float cosThetaT,
                                      Vec3& transVector)
{
    float lhsScalar = -cosThetaT + (nIDivNt * normDotI);
    Vec3 normScaled = isectNorm * lhsScalar;

    Vec3 vecToEyeScaled = vecToEye * nIDivNt;

    transVector = normScaled - vecToEyeScaled;
}


static inline void clamp(Vec3& rgb)
{
    if(rgb.x() > 1.0f)
        rgb.x() = 1.0f;
    if(rgb.y() > 1.0f)
        rgb.y() = 1.0f;
    if(rgb.z() > 1.0f)
        rgb.z() = 1.0f;
}

//bool RayCaster::test()
//{
//    Vec3 surfNorm(1.0f, 0.0f, 0.0f);
//    Vec3 vecToEye(1.0f, 1.0f, 0.0f);
//    vecToEye.normalize();
//
//    float nI = _environment.air().indexOfRefraction();
//    float mI = _environment.air().relativeMagPerm();
//
//    float nT = 1.65f;
//    float mT = 1.0f;
//
//    float normDotI = Vec3::dot(surfNorm, vecToEye);
//
//    float nIDivNt = nI / nT;
//    float mIDivMt = mI / mT;
//
//    float cosThetaT;//this value is reused in computing the transmission vector
//    float reflCoef = ComputeReflectionCoef(nIDivNt, mIDivMt, normDotI, cosThetaT);
//
//    float transCoef = 1.0f - reflCoef;
//
//    Vec3 reflVector;//this vector is normalized (as long as the isectNormal and vecToEye are normalized)
//    ComputeReflectionVector(vecToEye, surfNorm, normDotI, reflVector);
//
//    float iDotR = Vec3::dot(vecToEye, reflVector);
//
//    Vec3 transVector;//this vector is normalized (as long as vecToEye and isectNormal are normalized)
//    ComputeTransmissionVector(vecToEye, 
//                              surfNorm, 
//                              normDotI, 
//                              nIDivNt, 
//                              cosThetaT, 
//                              transVector);
//
//    float iDotT = Vec3::dot(vecToEye, transVector);
//
//    return true;
//}

void AddBlurOffset(Vec3& rayDir, float sigma)
{
    float theta = Random::Get(0.0f, (float)TwoPI);
    float u = Random::Get(0.0f, 1.0f);

    float sigmaSq2 = -2.0f * (sigma * sigma);
    float lnU = logf(u);
    float radicand = sqrtf(sigmaSq2 * lnU);
    float x = radicand * cosf(theta);
    float y = radicand * sinf(theta);

    Vec3 wVec(Random::Get(-9999.9f, 9999.9f),
              Random::Get(-9999.9f, 9999.9f),
              Random::Get(-9999.9f, 9999.9f));

    Vec3 uVec = Vec3::cross(rayDir, wVec);
    uVec.normalize();

    Vec3 vVec = Vec3::cross(rayDir, uVec);

    rayDir += (uVec*x);
    rayDir += (vVec*y);

    rayDir.normalize();
}

bool checkThicknessAndAttenOfTransObject(const Node* pTransIntoObj)
{
    if(pTransIntoObj->hasThickness())
        return true;

    //if transmitted object has no thickness, but its atten coef are zero then
    //return false
    const Vec3& atten = pTransIntoObj->surfaceProperties()->attenuationFactors();
    return(atten.x() > k_precEpsilon ||
           atten.y() > k_precEpsilon ||
           atten.z() > k_precEpsilon);
}

bool RayCaster::traceRay(const Ray& ray, Vec3& color, 
                         int maxDepth, const Node* pInsideObject) const
{
    if(maxDepth == 0)
        return false;
    --maxDepth;

    Node::IntersectionData data;

    bool intersection = false;
    if(pInsideObject)//if inside an object then only test against it
        intersection = pInsideObject->intersect(ray, data);
    else//otherwise test against entire scene
        intersection = _spSceneObjects->intersect(ray, data);

    if(intersection)
    {
        data.isectNorm.normalize();

        Vec3 surfNorm = data.isectNorm;
        if(data.isCurveShaded)
            surfNorm = data.shadingNorm;//this should be normalized already (see Triangle::normal(pt))

        float nI = _environment.air().indexOfRefraction();
        float mI = _environment.air().relativeMagPerm();

        float nT = data.spIntersectedObject->surfaceProperties()->indexOfRefraction();
        float mT = data.spIntersectedObject->surfaceProperties()->relativeMagPerm();

        Vec3 attenCoef;
        //assuming we are outside the object (i.e. inAir is true) then 
        //the reflected ray's origin should be pushed outside the object
        //to prevent false self intersection
        Vec3 reflOrigin = data.isectPt + (data.isectNorm * k_precEpsilon);
        //push the transmitted ray's origin inside the object
        Vec3 transOrigin = data.isectPt - (data.isectNorm * k_precEpsilon);
        //if(!inAir)
        if(pInsideObject != NULL)
        {
            nI = nT;
            nT = _environment.air().indexOfRefraction();
            mI = mT;
            mT = _environment.air().relativeMagPerm();

            Vec3 reflOriginCopy = reflOrigin;
            reflOrigin = transOrigin;
            transOrigin = reflOriginCopy;
            //surfNorm = -surfNorm;

            attenCoef = data.spIntersectedObject->surfaceProperties()->attenuationFactors();
        }
        else
        {
            attenCoef = _environment.air().attenuationFactors();
        }

        //assume the direction vector was normalized in previous function
        Vec3 vecToEye = -ray.direction();

        //float v2eLenSq = vecToEye.lengthSq();
        //if (fabs(v2eLenSq-1) > 0.1f)
        //{
        //    vecToEye.normalize();
        //    std::cerr << "vec2Eye not normal" << std::endl;
        //}
        //normDotI is used in computing the reflection coef, reflection vector, and transmission vector
        float normDotI = Vec3::dot(surfNorm, vecToEye);
        if(normDotI < 0.0f)
        {
            surfNorm = -surfNorm;
            normDotI = -normDotI;
        }
        else if(normDotI > 1.0f)
            normDotI = 1.0f;

        float nIDivNt = nI / nT;
        float mIDivMt = mI / mT;

        float cosThetaT;//this value is reused in computing the transmission vector
        float reflCoef = ComputeReflectionCoef(nIDivNt, mIDivMt, normDotI, cosThetaT);

        float transCoef = 1.0f - reflCoef;

        reflCoef *= data.spIntersectedObject->surfaceProperties()->specularCoef();
        transCoef *= data.spIntersectedObject->surfaceProperties()->specularCoef();

        Vec3 reflVector;//this vector is normalized (as long as the isectNormal and vecToEye are normalized)
        ComputeReflectionVector(vecToEye, surfNorm, normDotI, reflVector);
        //float refLenSq = reflVector.lengthSq();
        //if (fabs(refLenSq-1) > 0.1f)
        //{
        //    reflVector.normalize();
        //    std::cerr << "reflVector not normal" << std::endl;
        //}

        //if(inAir)
        if(pInsideObject == NULL)
        {
            computeLocalLighting(ray, 
                                 data, 
                                 reflVector, 
                                 reflCoef, 
                                 color);
        }

        attenCoef.x() = powf(attenCoef.x(), data.isectTVal);
        attenCoef.y() = powf(attenCoef.y(), data.isectTVal);
        attenCoef.z() = powf(attenCoef.z(), data.isectTVal);
        
        if(attenCoef.x() > 0.0f ||
           attenCoef.y() > 0.0f ||
           attenCoef.z() > 0.0f)//if attenCoef is zero then might as well skip reflection and transmission
        {
            float blurSize;
            float specExp = data.spIntersectedObject->surfaceProperties()->specularExp();
            if(specExp <= 0.0f)
                specExp = k_precEpsilon;//can't divide by zero, but small spec exp means large dispersion -> very blurry reflection

            if(reflCoef > 0.005f)
            {
                Vec3 reflColor;

                if(_drawBlurredRefl)
                {
                    blurSize = _blurReflFactor / specExp;

                    Vec3 avgColor;
                        
                    Ray reflRay(reflOrigin, reflVector);
                    for(size_t rayIndex = 0; rayIndex < _numBlurReflRays; ++rayIndex)
                    {
                        reflRay.direction() = reflVector;
                        AddBlurOffset(reflRay.direction(), blurSize);

                        Vec3 rayColor;
                        traceRay(reflRay, rayColor, maxDepth, pInsideObject);

                        avgColor += rayColor;
                    }

                    reflColor = avgColor;
                    reflColor /= static_cast<float>(_numBlurReflRays);
                }
                else
                {
                    Ray reflRay(reflOrigin, reflVector);

                    traceRay(reflRay, reflColor, maxDepth, pInsideObject);
                }

                reflColor *= reflCoef;
                color += reflColor;
            }

            if(transCoef > 0.005f && 
                (pInsideObject != NULL || checkThicknessAndAttenOfTransObject(data.spIntersectedObject.get())))
            {
                Vec3 transVector;//this vector is normalized (as long as vecToEye and isectNormal are normalized)
                ComputeTransmissionVector(vecToEye, 
                                          surfNorm, 
                                          normDotI, 
                                          nIDivNt, 
                                          cosThetaT, 
                                          transVector);

                

                const Node* pTransRayInsideObject = NULL;
                if(pInsideObject == NULL)//if in air
                {
                    //if intersected object has thickness then we are transmitting into the object
                    if(data.spIntersectedObject->hasThickness())
                        pTransRayInsideObject = data.spIntersectedObject.get();
                }
                else//inside an object - so transmitted ray should be back in air
                    pTransRayInsideObject = NULL;

                Vec3 transColor;

                if(_drawBlurredTrans)
                {
                    blurSize = _blurTransFactor / specExp;

                    Vec3 avgColor;
                        
                    Ray transRay(transOrigin, transVector);
                    for(size_t rayIndex = 0; rayIndex < _numBlurTransRays; ++rayIndex)
                    {
                        transRay.direction() = transVector;
                        AddBlurOffset(transRay.direction(), blurSize);
                        
                        Vec3 rayColor;
                        traceRay(transRay, rayColor, maxDepth, pTransRayInsideObject);
                
                        avgColor += rayColor;        
                    }

                    transColor = avgColor;
                    transColor /= static_cast<float>(_numBlurTransRays);
                }
                else
                {
                    //sometimes due to accumulated floating point precision loss
                    //the transmission vector comes out with greater than unit length
                    //so if that happens then normalize it
                    float transLenSq = transVector.lengthSq();
                    if (fabs(transLenSq-1) > 0.1f)
                    {
                        transVector.normalize();
                        //std::cerr << "transVector not normal" << std::endl;
                    }

                    Ray transRay(transOrigin, transVector);
                
                    traceRay(transRay, transColor, maxDepth, pTransRayInsideObject);
                }
                
                transColor *= transCoef;
                color += transColor;
            }
        }

        color *= attenCoef;
        //clamp between 0 and 1
        //clamp(color);
        return true;
    }

    return false;
}

static Vec3 AddSoftShadowOffset(const Vec3& shadowFeelerDir, float lightRadius)
{
    float theta = Random::Get(0, (float)TwoPI);

    float rSquaredDiv2 = (lightRadius * lightRadius * 0.5f);
    float u = Random::Get(0, rSquaredDiv2);

    float sqrt2U = sqrtf(2.0f * u);

    float x = sqrt2U * cosf(theta);
    float y = sqrt2U * sinf(theta);

    Vec3 wVec(Random::Get(-9999.9f, 9999.9f),
              Random::Get(-9999.9f, 9999.9f),
              Random::Get(-9999.9f, 9999.9f));

    Vec3 uVec = Vec3::cross(shadowFeelerDir, wVec);
    uVec.normalize();//shadowFeelerDir and wVec are not normalized so need to normalize this

    Vec3 vVec = Vec3::cross(shadowFeelerDir, uVec);
    vVec.normalize();//shadow feeler dir is not normalized so need to normalize uVec

    Vec3 offsetDir = shadowFeelerDir;
    offsetDir += (uVec * x);
    offsetDir += (vVec * y);

    return offsetDir;
}

bool RayCaster::isShadowed(float timeToLight, //incorporates radius of light
                           const Vec3& shadowFeelerOrigin, //vector from intersection to light => light.center - isectPt
                           const Vec3& offsetSFDir) const
{
    Ray shadowFeeler(shadowFeelerOrigin, offsetSFDir);
    Node::IntersectionData sfData;
    //if we intersect another object and it is in front of the light 
    //(time to intersection is less than time to light) then we are shadowed
    return (_spSceneObjects->intersect(shadowFeeler, sfData) &&  
            sfData.isectTVal < timeToLight);
}

void RayCaster::computeLocalLighting(const Ray& ray,
                                     const Node::IntersectionData& data,
                                     const Vec3& reflVector,
                                     float reflCoef,
                                     Vec3& color) const
{
    float specExp = data.spIntersectedObject->surfaceProperties()->specularExp();
    const Vec3& diffuse = data.spIntersectedObject->surfaceProperties()->diffuse();

    for(size_t lIndex = 0; lIndex < _lights.size(); ++lIndex)
    {
        const SmartPtr<Light>& spLight = _lights.at(lIndex);

        Vec3 shadowFeelerDir = spLight->center() - data.isectPt;
        
        Vec3 vecToLight = Vec3::normalize(shadowFeelerDir);
        //assume normal vector was normalized in calling function
        float normDotLight;
        if(data.isCurveShaded)
            normDotLight = Vec3::dot(data.shadingNorm, vecToLight);
        else
            normDotLight = Vec3::dot(data.isectNorm, vecToLight);

        if(normDotLight < 0.0f)
            continue; //light is behind object

        float distToLight = shadowFeelerDir.length();
        float timeToLight = 1.0f;
        
        Vec3 shadowFeelerOrigin = data.isectPt + (data.isectNorm * k_precEpsilon);

        float shadowFactor = 1.0f;
        float oneOverNumSFs = 1.0f / _numShadowFeelers;
        size_t shadowCount = 0;
        if(_drawSoftShadows)
        {
            //subtract radius from dist to light and corresponding time for time to light
            timeToLight -= (spLight->radius() / distToLight);
            distToLight -= spLight->radius();
            for(size_t sfIndex = 0; sfIndex < _numShadowFeelers; ++sfIndex)
            {
                Vec3 offsetSFDir = AddSoftShadowOffset(shadowFeelerDir, spLight->radius());

                if(isShadowed(timeToLight,
                              shadowFeelerOrigin,
                              offsetSFDir))
                {
                    shadowFactor -= oneOverNumSFs;
                    ++shadowCount;
                }
            }
        }
        //not doing soft shadows so just cast one ray directly at light point center
        else if(isShadowed(timeToLight,
                           shadowFeelerOrigin,
                           shadowFeelerDir))
        {
            shadowFactor = 0.0f; 
            shadowCount = _numShadowFeelers;
        }
            
        Vec3 curLightColor;
        if(shadowCount < _numShadowFeelers)
        {
            Vec3 attenCoef = _environment.air().attenuationFactors();
            attenCoef.x() = powf(attenCoef.x(), distToLight);
            attenCoef.y() = powf(attenCoef.y(), distToLight);
            attenCoef.z() = powf(attenCoef.z(), distToLight);

            Vec3 lightColor = spLight->color() * attenCoef;
          
            if(data.hasTexCoords)
            {
                const Vec3& tex = data.spIntersectedObject->surfaceProperties()->textureColor(data.texCoords);
                //diffuse * texture
                curLightColor.x() = (diffuse.x() * tex.x() * normDotLight * lightColor.x());
                curLightColor.y() = (diffuse.y() * tex.y() * normDotLight * lightColor.y());
                curLightColor.z() = (diffuse.z() * tex.z() * normDotLight * lightColor.z());
            }
            else
            {
                //just diffuse
                curLightColor.x() = (diffuse.x() * normDotLight * lightColor.x());
                curLightColor.y() = (diffuse.y() * normDotLight * lightColor.y());
                curLightColor.z() = (diffuse.z() * normDotLight * lightColor.z());
            }

            float rVDotL = Vec3::dot(reflVector, vecToLight);
            if(rVDotL < 0.0f)
                rVDotL = 0.0f;

            float specularFactor = reflCoef * powf(rVDotL, specExp);

            //specular
            curLightColor.x() += (specularFactor * lightColor.x());
            curLightColor.y() += (specularFactor * lightColor.y());
            curLightColor.z() += (specularFactor * lightColor.z());

            curLightColor *= shadowFactor;
        }

        color += curLightColor;
    }

    //ambient
    if(data.hasTexCoords)
    {
        const Vec3& tex = data.spIntersectedObject->surfaceProperties()->textureColor(data.texCoords);   
        color.x() += (diffuse.x() * tex.x() * _environment.ambientLight().x());
        color.y() += (diffuse.y() * tex.y() * _environment.ambientLight().y());
        color.z() += (diffuse.z() * tex.z() * _environment.ambientLight().z());
    }
    else
    {
        color.x() += (diffuse.x() * _environment.ambientLight().x());
        color.y() += (diffuse.y() * _environment.ambientLight().y());
        color.z() += (diffuse.z() * _environment.ambientLight().z());
    }
}

void RayCaster::computeColor(const Ray& ray,
                             const Node::IntersectionData& data,
                             Vec3& color) const
{
    Vec3 unitVecRevDir = Vec3::normalize(ray.direction());
    unitVecRevDir *= -1.0f;
    //Compute Color
    color = data.spIntersectedObject->surfaceProperties()->diffuse();
    
    //assume isectNorm already normalized by calling function
    float normDotRevRayVec = Vec3::dot(data.isectNorm, unitVecRevDir);
    if(normDotRevRayVec < 0.0f)
    {
        normDotRevRayVec = -normDotRevRayVec;
    }
    color.x() *= normDotRevRayVec;
    color.y() *= normDotRevRayVec;
    color.z() *= normDotRevRayVec;
}

void RayCaster::addSceneObject(Node* pSceneObject)
{
    _spSceneObjects->addChild(pSceneObject);
}

void RayCaster::addLight(Light* pLight)
{
    _lights.push_back(pLight);
}

void RayCaster::setCamera(Camera* pCamera)
{
    _spCamera = pCamera;
}

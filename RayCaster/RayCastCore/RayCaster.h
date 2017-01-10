#pragma once

#include "Group.h"
#include "Camera.h"
#include "Light.h"
#include "Environment.h"
#include "SmartPtr.h"
#include "KDTree.h"
#include "SphereTree.h"

#include <string>

namespace cs500
{
    class RayCaster
    {
    public:
        RayCaster() : 
          _numShadowFeelers(2), _drawSoftShadows(false), 
          _blurReflFactor(0.15f), _numBlurReflRays(1), _drawBlurredRefl(false), 
          _blurTransFactor(0.15f), _numBlurTransRays(1), _drawBlurredTrans(false) {}

        bool loadScene(const std::string& inputFile);
        
        void addSceneObject(Node* pSceneObject);
        void addLight(Light* pLight);
        void setCamera(Camera* pCamera);

        const Camera* getCamera() const { return _spCamera.get(); }

        Group* getSceneRoot() { return _spSceneObjects.get(); }
        Node* getSceneObject(size_t index) { return _spSceneObjects->getChild(index); }
        size_t getNumSceneObjects() { return _spSceneObjects->getNumChildren(); }

        const Environment& getEnvironment() const { return _environment; }
        Environment& getEnvironment() { return _environment; }
        //bool test();
        bool traceRay(const Ray& ray, Vec3& color, int maxDepth, const Node* pInsideObject=NULL) const;

        size_t getNumShadowFeelers() const { return _numShadowFeelers; }
        float getBlurReflFactor() const { return _blurReflFactor; }
        float getBlurTransFactor() const { return _blurTransFactor; }

        void setDrawSoftShadows(bool flag, size_t numShadowFeelers=2) 
        { 
            _drawSoftShadows = (flag && numShadowFeelers>0); 
            if(_drawSoftShadows)
                _numShadowFeelers = numShadowFeelers;
            else
                _numShadowFeelers = 1;//set this to one so that the shadow factor is correctly computed
        }

        void setDrawBlurredReflections(bool flag, size_t numBlurRays=1, float blurFactor=0.15f) 
        { _drawBlurredRefl = (flag && numBlurRays); _numBlurReflRays = numBlurRays; _blurReflFactor = blurFactor; }

        void setDrawBlurredTransmissions(bool flag, size_t numBlurRays=1, float blurFactor=0.15f) 
        { _drawBlurredTrans = (flag && numBlurRays); _numBlurTransRays = numBlurRays; _blurTransFactor = blurFactor; }

        void buildSphereTree();
        void buildKDTree();
        
    private:
        void computeLocalLighting(const Ray& ray,
                                  const Node::IntersectionData& data,
                                  const Vec3& reflVector,
                                  float reflCoef,
                                  Vec3& color) const;

        bool isShadowed(float timeToLight, //incorporates radius of light
                        const Vec3& shadowFeelerOrigin,
                        const Vec3& shadowFeelerDir) const; //vector from intersection to light => light.center - isectPt

        void computeColor(const Ray& ray,
                          const Node::IntersectionData& data,
                          Vec3& color) const;

        SmartPtr<Group> _spSceneObjects;
        SmartPtr<Camera> _spCamera;
        
        typedef std::vector< SmartPtr<Light> > Lights;
        Lights _lights;

        Environment _environment;

        size_t _numShadowFeelers;
        bool _drawSoftShadows;

        float _blurReflFactor;
        size_t _numBlurReflRays;
        bool _drawBlurredRefl;

        float _blurTransFactor;
        size_t _numBlurTransRays;
        bool _drawBlurredTrans;
    };
};
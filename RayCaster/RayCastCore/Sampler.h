#pragma once

#include "Camera.h"
#include "SmartPtr.h"
#include "RayCaster.h"
#include "Vec3.h"

namespace cs500
{   
    //foward declaration
    class Image;

    class Sampler
    {
    public:
        Sampler() : _pixelWidth(0.0f), _pixelHeight(0.0f), _tileSize(32), _renderTiles(false) {}
        Image* generateImage(const Camera& cam,
                             RayCaster& rayCaster,
                             int horizResolution,
                             int maxDepth=10);
    protected:
        virtual void initialize(const Camera& cam, int horizRes);
        virtual void computePixelColor(int row, 
                                       int col, 
                                       int maxDepth, 
                                       const RayCaster& rayCaster, 
                                       Vec3& color) const;
        virtual void generateImagePixels(Image& image, 
                                        RayCaster& rayCaster,
                                        int horizResolution, 
                                        int vertResolution, 
                                        int maxDepth);
        void computeImagePixels(int rowStart, int rowCount, int colStart, int colCount,
                                 Image& image, 
                                 RayCaster& rayCaster, 
                                 int maxDepth);
    public:
        const Camera& camera() const { return *_spCamera.get(); }
        const Vec3& eyePos() const { return _eyePosition; }
        float pixelWidth() const { return _pixelWidth; }
        float pixelHeight() const { return _pixelHeight; }
        void setRenderTiles(bool flag, int tileSize=32)
        {
            _renderTiles = flag && tileSize > 0;
            _tileSize = tileSize;
        }
    private:
        SmartPtr<const Camera> _spCamera;
        Vec3 _eyePosition;
        float _pixelWidth;
        float _pixelHeight;
        int _tileSize;
        bool _renderTiles;
    };
};
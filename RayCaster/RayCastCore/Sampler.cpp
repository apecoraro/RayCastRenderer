#include "Sampler.h"
#include "RayCaster.h"
#include "Ray.h"
#include "Image.h"

using namespace cs500;

void Sampler::computePixelColor(int row, 
                                int col, 
                                int maxDepth, 
                                const RayCaster& rayCaster, 
                                Vec3& color) const
{
    float beta = 1.0f;

    Vec3 betaV = _spCamera->vAxis();
    betaV *= (beta - (row * _pixelHeight));

    float alpha = -1.0f;

    Vec3 alphaU = _spCamera->uAxis();
    alphaU *= (alpha + (col * _pixelWidth));

    Vec3 viewPlanePoint = _spCamera->center();
    viewPlanePoint += betaV;
    viewPlanePoint += alphaU;

    Ray ray(_eyePosition, Vec3::normalize(viewPlanePoint - _eyePosition));

    rayCaster.traceRay(ray, color, maxDepth);
}

void Sampler::initialize(const Camera& cam, int horizRes)
{
    _spCamera = &cam;
    float aspectRatio = _spCamera->vAxis().length() / _spCamera->uAxis().length();
    int vertResolution = (int)(static_cast<float>(horizRes) * aspectRatio);

    _pixelWidth = 2.0f/(float)horizRes;
    _pixelHeight = 2.0f/(float)vertResolution;

    _eyePosition = _spCamera->center() + _spCamera->eyeVec();
}

Image* Sampler::generateImage(const Camera& cam,
                              RayCaster& rayCaster,
                              int horizResolution,
                              int maxDepth)//=10
{
    initialize(cam, horizResolution);
    rayCaster.getSceneRoot()->computeProjectedSize(cam, horizResolution);

    float aspectRatio = cam.vAxis().length() / cam.uAxis().length();
    int vertResolution = (int)(static_cast<float>(horizResolution) * aspectRatio);

    Image* pImage = new Image(horizResolution, vertResolution);
    
    generateImagePixels(*pImage, rayCaster, horizResolution, vertResolution, maxDepth);

    return pImage;
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

void Sampler::generateImagePixels(Image& image, 
                                  RayCaster& rayCaster,
                                  int horizResolution, 
                                  int vertResolution, 
                                  int maxDepth)
{
    if(_renderTiles)
    {
        int numTileRows = vertResolution / _tileSize;
        int numTileCols = horizResolution / _tileSize;
        for(int tileRow = 0; tileRow < numTileRows; ++tileRow)
        {
            int rowStart = tileRow * _tileSize;
            for(int tileCol = 0; tileCol < numTileCols; ++tileCol)
            {
                int colStart = tileCol * _tileSize;
                computeImagePixels(rowStart, _tileSize,
                                   colStart, _tileSize,
                                   image,
                                   rayCaster, maxDepth);
            }
        }
        //do left overs
        int leftOverCols = horizResolution % _tileSize;
        computeImagePixels(0, vertResolution, 
                           _tileSize*numTileCols, leftOverCols,
                           image, 
                           rayCaster,
                           maxDepth);

        int leftOverRows = vertResolution % _tileSize;
        computeImagePixels(numTileRows*_tileSize, leftOverRows,
                           0, horizResolution,
                           image,
                           rayCaster, maxDepth);
    }
    else
    {
        computeImagePixels(0, vertResolution, 
                           0, horizResolution,
                           image, 
                           rayCaster,
                           maxDepth);
        
    }
}

void Sampler::computeImagePixels(int rowStart, int rowCount, int colStart, int colCount,
                                 Image& image, 
                                 RayCaster& rayCaster, 
                                 int maxDepth)
{
    int stopRow = rowStart + rowCount;
    int stopCol = colStart + colCount;
    for(int row = rowStart; row < stopRow; ++row)
    {
        for(int col = colStart; col < stopCol; ++col)
        {
            Vec3 color;

            computePixelColor(row, col, maxDepth, rayCaster, color);

            clamp(color);

            unsigned char r = (unsigned char)(255.0f * color.x());
            unsigned char g = (unsigned char)(255.0f * color.y());
            unsigned char b = (unsigned char)(255.0f * color.z());

            image.setPixel(row, col, r, g, b);
        }
    }
}
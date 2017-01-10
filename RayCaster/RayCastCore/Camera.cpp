#include "Camera.h"

using namespace cs500;

Camera::Camera(const Vec3& center,
               const Vec3& uVec,
               const Vec3& vVec,
               const Vec3& eyeVec) :
    _center(center),
    _uVec(uVec),
    _vVec(vVec),
    _eyeVec(eyeVec),
    _eyePos(center + eyeVec)
{
}

float Camera::pixelSize(int horizRes) const
{
    float aspectRatio = vAxis().length() / uAxis().length();
    int vertResolution = (int)(static_cast<float>(horizRes) * aspectRatio);

    float halfPixelWidth = (1.0f/(float)horizRes);
    float halfPixelHeight = (1.0f/(float)vertResolution);

    Vec3 viewPlaneLL = center() - (vAxis() * halfPixelHeight) - (uAxis() * halfPixelWidth);
    Vec3 camToVPLL(viewPlaneLL - eyePos());
    camToVPLL.normalize();

    Vec3 viewPlaneUR = center() + (vAxis() * halfPixelHeight) + (uAxis() * halfPixelWidth);
    Vec3 camToVPUR(viewPlaneUR - eyePos());
    camToVPUR.normalize();

    return 1.0f - Vec3::dot(camToVPLL, camToVPUR);
}
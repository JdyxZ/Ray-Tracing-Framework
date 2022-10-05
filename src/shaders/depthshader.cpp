#include "depthshader.h"
#include "../core/utils.h"

DepthShader::DepthShader() :
    color(Vector3D(0, 1, 0))
{ }

DepthShader::DepthShader(Vector3D color_, double maxDist_, Vector3D bgColor_) :
    Shader(bgColor_), color(color_)
{ }

Vector3D computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList)
{
    Intersection intersection = Intersection();
    intersection.itsPoint = Vector3D(INT32_MAX, INT32_MAX, INT32_MAX);
    if (Utils::getClosestIntersection(r, objList, intersection))
        return hitColor;
    else
        return bgColor;
}
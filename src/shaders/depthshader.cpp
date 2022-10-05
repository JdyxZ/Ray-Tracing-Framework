#include "depthshader.h"
#include "../core/utils.h"

DepthShader::DepthShader() :
    color(Vector3D(0, 1, 0))
{ }

DepthShader::DepthShader(Vector3D color_, double maxDist_, Vector3D bgColor_) :
    Shader(bgColor_), maxDist(maxDist_), color(color_)
{ }

Vector3D DepthShader::computeColor(const Ray & r, const std::vector<Shape*>&objList, const std::vector<PointLightSource>&lsList) const
{
    Intersection intersection = Intersection();
    if (Utils::getClosestIntersection(r, objList, intersection))
    {
        double hit_distance = Utils::computeDistance(intersection.itsPoint,r.o);
        return color * Vector3D(max(1 - hit_distance / maxDist, 0.0));
    }
    else
            return bgColor;
}

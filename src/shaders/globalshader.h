#pragma once
#include "shader.h"
#include "../core/utils.h"

class GlobalShader : public Shader
{
public:
    GlobalShader() {};
    GlobalShader(Vector3D bgColor_, Vector3D ambient_term_);

    Vector3D computeColor(const Ray& r,const std::vector<Shape*>& objList,const std::vector<PointLightSource>& lsList) const;
    Vector3D computePhong(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const;
    Vector3D computeMirror(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const;
    Vector3D computeTransmissive(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const;
    Vector3D computeGlobalIllumination(const Intersection& i) const;

private:
    Vector3D ambient_term = Vector3D(0.1);
};

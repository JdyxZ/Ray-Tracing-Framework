#pragma once
#include "shader.h"
#include "../core/utils.h"

enum Bounces
{
    Two_Bounces,
    N_Bounces
};

class ExplicitGlobalShader : public Shader
{
public:
    ExplicitGlobalShader() {};
    ExplicitGlobalShader(Vector3D bgColor_, Vector3D ambient_term_, Bounces bounces_, int nSamples_, int maxDepth_);

    Vector3D computeColor(const Ray& r,const std::vector<Shape*>& objList,const std::vector<PointLightSource>& lsList) const;
    Vector3D computePhong(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const;
    Vector3D computeMirror(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const;
    Vector3D computeTransmissive(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const;

private:
    Vector3D ambient_term = Vector3D(0.1);
    Bounces bounces = Two_Bounces;
    const int nSamples = 10;
    const int maxDepth = 5;
};

#pragma once
#include "shader.h"
#include "../core/utils.h"

class DirectShader : public Shader
{
public:
    DirectShader() {};
    DirectShader(Vector3D bgColor_);

    Vector3D computeColor(const Ray& r,const std::vector<Shape*>& objList,const std::vector<PointLightSource>& lsList) const;

}
;

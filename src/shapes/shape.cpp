#include "shape.h"

Shape::Shape(const Matrix4x4 &t_, Material *material_)
{
    objectToWorld = t_;
    objectToWorld.inverse(worldToObject);
    material = material_;
}

Material& Shape::getMaterial() const
{
    return *material;
}

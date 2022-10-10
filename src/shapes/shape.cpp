#include "shape.h"

Shape::Shape(Matrix4x4 *t_, Material *material_)
{
    center = t_->getTranslation();
    objectToWorld = *t_;
    objectToWorld.inverse(worldToObject);
    material = material_;
}

const Material& Shape::getMaterial() const
{
    return *material;
}

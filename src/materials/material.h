#ifndef MATERIAL
#define MATERIAL

#include "../core/vector3d.h"

class Material
{
public:
    Vector3D kd;
    Vector3D ks;
    double shininess;

    Material();

    virtual Vector3D getReflectance(const Vector3D &n, const Vector3D &wo,const Vector3D &wi) = 0;
    virtual bool hasSpecular() = 0;
    virtual bool hasTransmission() = 0;
    virtual bool hasDiffuseOrGlossy() = 0;
    virtual double getIndexOfRefraction() const;
};


#endif // MATERIAL

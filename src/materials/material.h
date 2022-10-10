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

    virtual Vector3D getReflectance(const Vector3D &n, const Vector3D &wo,const Vector3D &wi) const = 0;
    virtual bool hasSpecular() const = 0 ;
    virtual bool hasTransmission() const = 0;
    virtual bool hasDiffuseOrGlossy() const = 0;
    virtual double getIndexOfRefraction() const;
};


#endif // MATERIAL

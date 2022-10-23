#ifndef MATERIAL
#define MATERIAL

#include "../core/vector3d.h"

class Material
{
public:
    
    Material();

    //Methods
    static Vector3D getPerfectReflection(const Vector3D& n, const Vector3D& wo);

    //Pure virtual methods
    virtual Vector3D getReflectance(const Vector3D &n, const Vector3D &wo,const Vector3D &wi) const = 0;
    virtual bool hasSpecular() const = 0 ;
    virtual bool hasTransmission() const = 0;
    virtual bool hasDiffuseOrGlossy() const = 0;
    virtual double getIndexOfRefraction() const;
    virtual Vector3D getDiffuseCoefficient() const;
};

#endif // MATERIAL

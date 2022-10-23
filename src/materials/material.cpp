#include "material.h"

#include <iostream>

Material::Material()
{ }

Vector3D Material::getPerfectReflection(const Vector3D & n, const Vector3D & wo)
{
    const Vector3D wr = (n * 2 * dot(wo, n) - wo).normalized();
    return wr;
}

double Material::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;

    return -1;
}

Vector3D Material::getDiffuseCoefficient() const
{
    std::cout << "Warning !"
        << "Calling \" Material::getDiffuseCoefficient() \""
        << "for a non−diffuse or non−glossy material "
        << std::endl;

    return Vector3D(-1.0);
}

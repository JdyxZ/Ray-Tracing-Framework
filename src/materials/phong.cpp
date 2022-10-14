#include "phong.h"

Phong::Phong() :
    shininess(50)
{
    kd = Vector3D(1.0);
    ks = Vector3D(1.0);
}

Phong::Phong(Vector3D kd_, Vector3D ks_, double shininess_) :
    shininess(shininess_), kd(kd_), ks(ks_)
{

}



bool Phong::hasSpecular() const
{
    // This material does not have specular component
    return false;
}

bool Phong::hasTransmission() const
{
    return false;
}

bool Phong::hasDiffuseOrGlossy() const
{
    return true;
}

Vector3D Phong::getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi) const
{
    // Compute the perfect reflection direction
    Vector3D wr = n * 2 * dot(n, wi) - wi;
    // Compute the cos of the angle between the incident light direction
    // and the perfect reflection direction
    double cosWrWo = dot(wr, wo);
    return kd * std::max(dot(wi, n), 0.0) + ks * pow(cosWrWo, shininess);
}

Phong::~Phong()
{

}
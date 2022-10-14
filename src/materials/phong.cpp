#include "phong.h"
#include "../core/utils.h"

Phong::Phong(Vector3D kd, Vector3D ks, double shininess)
{
	this->kd = kd;
	this->ks = ks;
	this->shininess = shininess;
}

Vector3D Phong::getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi) const
{
	Vector3D wr = Utils::computeReflectionDirection(-wi, n);
	Vector3D reflectance = kd * dot(wi,n) + ks * pow(dot(wo, wr), shininess);
	return reflectance;
}

bool Phong::hasDiffuseOrGlossy() const
{
	return kd.x > 0.0 && kd.y > 0.0 && kd.z > 0.0;
}


bool Phong::hasSpecular() const
{
	return ks.x > 0.0 && ks.y > 0.0 && ks.z > 0.0;
}

bool Phong::hasTransmission() const
{
	return hasDiffuseOrGlossy() && hasSpecular() && shininess > 0.0;
}

double Phong::getIndexOfRefraction() const
{
	return 0.0;
}
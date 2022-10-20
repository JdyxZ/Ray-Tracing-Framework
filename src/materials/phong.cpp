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

bool Phong::hasSpecular() const
{
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

double Phong::getIndexOfRefraction() const
{
	return 0.0;
}
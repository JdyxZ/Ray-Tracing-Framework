#include "phong.h"

Phong::Phong(Vector3D kd, Vector3D ks, double shininess)
{
	this->kd = kd;
	this->ks = ks;
	this->shininess = shininess;
}

Vector3D Phong::getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi)
{
	Vector3D wr = ((n * wi) * 2) * n - wi;
	Vector3D reflectance = (wi * n) * kd + ((wo * wr) ^ shininess) * ks;
	return reflectance;
}

bool Phong::hasDiffuseOrGlossy()
{
	return kd.x > 0.0 && kd.y > 0.0 && kd.z > 0.0;
}


bool Phong::hasSpecular()
{
	return ks.x > 0.0 && ks.y > 0.0 && ks.z > 0.0;
}

bool Phong::hasTransmission()
{
	return hasDiffuseOrGlossy() && hasSpecular() && shininess > 0.0;
}

double Phong::getIndexOfRefraction()
{
	return 0.0;
}
#pragma once
#include "material.h"

class Phong : public Material
{
public:
	Phong() { kd = Vector3D(), ks = Vector3D(), shininess = 0.0; }
	Phong(Vector3D kd, Vector3D ks, double shininess);

	//Inherited methods
	Vector3D getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi);
	bool hasSpecular();
	bool hasTransmission();
	bool hasDiffuseOrGlossy();
	double getIndexOfRefraction();
};


#pragma once
#include "material.h"

class Transmissive : public Material
{
public:

	//Transmissive parameters
	double refractive_index;

	//Constructor
	Transmissive() { refractive_index = 0.0; }
	Transmissive(double refractive_index);

	//Inherited methods
	Vector3D getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi) const;
	bool hasSpecular() const;
	bool hasTransmission() const;
	bool hasDiffuseOrGlossy() const;
	double getIndexOfRefraction() const;
	Vector3D getDiffuseCoefficient() const;

};


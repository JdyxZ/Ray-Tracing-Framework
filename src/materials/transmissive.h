#pragma once
#include "material.h"

class Transmissive : public Material
{
public:

	//Transmissive parameters
	double n_t;

	//Constructor
	Transmissive(double n_t);

	//Inherited methods
	Vector3D getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi) const;
	bool hasSpecular() const;
	bool hasTransmission() const;
	bool hasDiffuseOrGlossy() const;
	double getIndexOfRefraction() const;
};
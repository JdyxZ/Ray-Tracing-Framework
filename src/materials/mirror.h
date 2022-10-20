#include "material.h"

class Mirror : public Material
{
public:
	//Constructor
	Mirror() { }

	//Inherited methods
	Vector3D getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi) const;
	bool hasSpecular() const;
	bool hasTransmission() const;
	bool hasDiffuseOrGlossy() const;
	double getIndexOfRefraction() const;
};
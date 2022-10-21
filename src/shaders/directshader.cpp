#include "directshader.h"
#include "../core/utils.h"

DirectShader::DirectShader(Vector3D bgColor_) :
    Shader(bgColor_)
{ }
   
Vector3D DirectShader::computeColor(const Ray& r, const std::vector<Shape*>&objList, const std::vector<PointLightSource>&lsList) const
{

    //Create an intersection instance
    Intersection intersection = Intersection();

    //Check if the camera sees a shape
    if (!Utils::getClosestIntersection(r, objList, intersection)) return bgColor;

    //Check material
    if (intersection.shape->getMaterial().hasDiffuseOrGlossy()) return computePhong(r, intersection, objList, lsList);
    if (intersection.shape->getMaterial().hasSpecular()) return computeMirror(r, intersection, objList, lsList);
    if (intersection.shape->getMaterial().hasTransmission()) return computeTransmissive(r, intersection, objList, lsList);    
}

Vector3D DirectShader::computePhong(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const
{
    //Declare variables
    const Shape* shape = i.shape;
    const Vector3D n = i.normal.normalized();
    const Vector3D p = i.itsPoint;
    const Vector3D wo = -r.d.normalized();
    Vector3D wi;
    double light_distance;
    Vector3D incident_light;
    Vector3D reflectance;
    bool visibility;
    Vector3D color = Vector3D();

    //For each light
    for (const PointLightSource& l : lsList)
    {
        //Light vector
        wi = (l.getPosition() - p);
        light_distance = wi.length();
        wi /= light_distance;

        //NdotL
        const double NdotL = dot(n, wi);

        //Visibility
        visibility = !Utils::hasIntersection(Ray(p, wi, 0, Epsilon, light_distance), objList);

        //Check that light is hitting the above surface of the shape
        if (visibility)
        {
            //Incident light
            incident_light = l.getIntensity(p);

            //Reflectance
            reflectance = i.shape->getMaterial().getReflectance(n, wo, wi);

            //Direct illumination
            color += incident_light * reflectance * NdotL;
        }
    }

    return color;
}

Vector3D DirectShader::computeMirror(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const
{
    //Declare variables
    const Vector3D n = i.normal.normalized();
    const Vector3D p = i.itsPoint;
    const Vector3D wo = -r.d.normalized();

    //Compute reflection direction
    const Vector3D wr = (n * 2 * dot(wo, n) - wo).normalized();
    const Ray reflectionRay = Ray(p, wr, r.depth + 1);
    
    //Output
    return computeColor(reflectionRay, objList, lsList);

}

Vector3D DirectShader::computeTransmissive(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const
{
    //Declare variables
    const Shape* shape = i.shape;
    const Vector3D n = i.normal.normalized();
    const Vector3D p = i.itsPoint;
    const Vector3D wo = -r.d;
    const double WOdotN = dot(wo, n);

    //Set refractive index with regard to medium change
    double refractive_index = shape->getMaterial().getIndexOfRefraction();
    if (WOdotN < 0) refractive_index = 1 / refractive_index;

    //Compute totalInternalReflection
    const double radicant = 1 - pow(refractive_index, 2) * (1 - pow(WOdotN, 2));
    bool totalInternalReflection = (radicant < 0);

    //Check whether the ray reflects or refracts
    switch (totalInternalReflection)
    {
    case(0):
        {
            //Compute refraction direction
            const Vector3D wt = (n * (-sqrt(radicant) + refractive_index * WOdotN) - wo * refractive_index).normalized();
            const Ray refractedRay = Ray(p, wt, r.depth + 1);
            return computeColor(refractedRay, objList, lsList);
        }
    case(1):  
        {
            //Compute specular reflection
            return computeMirror(r, i, objList, lsList);
        }
    }    

}

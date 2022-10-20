#include "directshader.h"
#include "../core/utils.h"

DirectShader::DirectShader(Vector3D bgColor_) :
    Shader(bgColor_)
{ }
   
Vector3D DirectShader::computeColor(const Ray & r, const std::vector<Shape*>&objList, const std::vector<PointLightSource>&lsList) const
{
    //Create an intersection instance
    Intersection intersection = Intersection();

    //Check if the camera sees a shape
    if (Utils::getClosestIntersection(r, objList, intersection))
    {
        //Declare variables
        const Shape* shape = intersection.shape;
        const Vector3D n = intersection.normal.normalized();
        const Vector3D p = intersection.itsPoint;
        const Vector3D wo = -r.d.normalized();
        Vector3D wi;
        double light_distance;
        Vector3D incident_light;
        Vector3D reflectance;
        bool visibility;
        Vector3D color = Vector3D();

        //Check material
        if (!shape->getMaterial().hasDiffuseOrGlossy())
            return Vector3D();

        //For each light
        for(const PointLightSource& l : lsList)
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
                reflectance = intersection.shape->getMaterial().getReflectance(n,wo,wi);

                //Direct illumination
                color += incident_light * reflectance * NdotL;
            }
        }

        return color;
    }
    else
        return bgColor;
    
}
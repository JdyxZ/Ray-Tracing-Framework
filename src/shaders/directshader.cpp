#include "directshader.h"

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
        Vector3D n = intersection.normal.normalized();
        Vector3D p = intersection.itsPoint;
        Vector3D wo = -r.d.normalized();
        Vector3D wi;
        Vector3D incident_light;
        Vector3D reflectance;
        bool visibility;
        Vector3D color = Vector3D();

        //For each light
        for (int i = 0; i < lsList.size(); i++) //Mejorar loop para evitar crear iterativamente una instancia de PointLightSource.
        {
            PointLightSource l = lsList[i];
            wi = (l.getPosition() - p).normalized();
            visibility = dot(n, wi) > 0.0 ? true : false;

            //Check that light is hitting the above surface of the shape
            if (visibility)
            {
                //Incident light
                incident_light = l.getIntensity(p);

                //Reflectance
                reflectance = intersection.shape->getMaterial().getReflectance(n,wo,wi);

                //Direct illumination
                color += incident_light * reflectance;
            }
        }
        
        //Clamp the color to avoid artifacts
        return color.clamp();
    }
    else
        return bgColor;
    
}
#include "explicitglobalshader.h"
#include "../core/utils.h"
#include "../core/hemisphericalsampler.h"
#include <math.h>
   
ExplicitGlobalShader::ExplicitGlobalShader(Vector3D bgColor_, Vector3D ambient_term_, Bounces bounces_, int nSamples_, int maxDepth_):

    Shader(bgColor_), ambient_term(ambient_term_), bounces(bounces_), nSamples(nSamples_), maxDepth(maxDepth_)
{ }

Vector3D ExplicitGlobalShader::computeColor(const Ray& r, const std::vector<Shape*>&objList, const std::vector<PointLightSource>&lsList) const
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

Vector3D ExplicitGlobalShader::computePhong(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const
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
    const Vector3D kd = i.shape->getMaterial().getDiffuseCoefficient();
    Vector3D color = Vector3D();

    //Direct illumination
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

            //Direct illumination equation
            color += incident_light * reflectance * NdotL;
        }
    }

    //Global illumination
    switch (bounces)
    {
        case(Two_Bounces):
        {
            if (r.depth == 0)
            {
                //Declare variables
                Vector3D outgoing_radiance = Vector3D();
                const HemisphericalSampler* sampler = new HemisphericalSampler();

                for (int j = 0; j < nSamples; j++)
                {
                    //Sampling direction
                    Vector3D sampling_direction = sampler->getSample(n);
                    const double NdotS = dot(n, sampling_direction);
                    const Ray sampling_ray = Ray(p, sampling_direction, r.depth + 1);

                    //Incoming radiance
                    Vector3D incoming_radiance = computeColor(sampling_ray, objList, lsList);

                    //Reflectance
                    Vector3D reflectance = i.shape->getMaterial().getReflectance(n, wo, sampling_direction);

                    //Outgoing radiance
                    outgoing_radiance += incoming_radiance * reflectance * NdotS;

                }
                //Result
                outgoing_radiance *= 1 / (2 * M_PI * nSamples);
                color += outgoing_radiance;

            }
            else 
            {
                //Approximation
                color += kd * ambient_term;
            }
            break;

        }
        case(N_Bounces):
        {
            if (r.depth == 0)
            {
                //Declare variables
                Vector3D outgoing_radiance = Vector3D();
                const HemisphericalSampler* sampler = new HemisphericalSampler();

                for (int j = 0; j < nSamples; j++)
                {
                    //Sampling direction
                    Vector3D sampling_direction = sampler->getSample(n);
                    const double NdotS = dot(n, sampling_direction);
                    const Ray sampling_ray = Ray(p, sampling_direction, r.depth + 1);

                    //Incoming radiance
                    Vector3D incoming_radiance = computeColor(sampling_ray, objList, lsList);

                    //Reflectance
                    Vector3D reflectance = i.shape->getMaterial().getReflectance(n, wo, sampling_direction);

                    //Outgoing radiance
                    outgoing_radiance += incoming_radiance * reflectance * NdotS;

                }
                //Result
                outgoing_radiance *= 1 / (2 * M_PI * nSamples);
                color += outgoing_radiance;
                 
             }
            else if(r.depth == maxDepth)
            {
                //Approximation
                color += kd * ambient_term;
            }
            else
            {
                //Declare variables
                Vector3D outgoing_radiance = Vector3D();

                //Result
                color += outgoing_radiance;
     
            }
            break;
        }
    }
  
    //Output
    return color;
}

Vector3D ExplicitGlobalShader::computeMirror(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const
{
    //Declare variables
    const Vector3D n = i.normal.normalized();
    const Vector3D p = i.itsPoint;
    const Vector3D wo = -r.d.normalized();

    //Compute reflection direction
    const Vector3D wr = (n * 2 * dot(wo, n) - wo).normalized();
    const Ray reflectionRay = Ray(p, wr, r.depth);
    
    //Output
    return computeColor(reflectionRay, objList, lsList);

}

Vector3D ExplicitGlobalShader::computeTransmissive(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const
{
    //Declare variables
    const Shape* shape = i.shape;
    Vector3D n = i.normal.normalized();
    Vector3D p = i.itsPoint;
    Vector3D wo = -r.d;
    double WOdotN = dot(wo, n);
    double refractive_index = shape->getMaterial().getIndexOfRefraction();

    //Set refractive index with regard to medium change
    if (WOdotN < 0)
    {
        n = -n;
        WOdotN = dot(wo, n);
        refractive_index = 1 / refractive_index;
    }

    //Compute totalInternalReflection
    double radicant = 1 - pow(refractive_index, 2) * (1 - pow(WOdotN, 2));
    bool totalInternalReflection = (radicant < 0);

    //Check whether the ray reflects or refracts
    switch (totalInternalReflection)
    {
    case(0):
    {
        //Compute refraction direction
        const Vector3D wt = (n * (-sqrt(radicant) + refractive_index * WOdotN) - wo * refractive_index).normalized();
        const Ray refractedRay = Ray(p, wt, r.depth);
        return computeColor(refractedRay, objList, lsList);
    }
    case(1):
    {
        //Compute specular reflection
        return computeMirror(r, i, objList, lsList);
    }
    }

}

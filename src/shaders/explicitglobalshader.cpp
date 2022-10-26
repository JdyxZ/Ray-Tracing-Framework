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
    Intersection i = Intersection();

    //Check if the camera sees a shape
    if (!Utils::getClosestIntersection(r, objList, i)) return bgColor;

    //Check material
    if (i.shape->getMaterial().hasDiffuseOrGlossy()) return computePhong(r, i, objList, lsList);
    else if (i.shape->getMaterial().hasSpecular()) return computeMirror(r, i, objList, lsList);
    else if (i.shape->getMaterial().hasTransmission()) return computeTransmissive(r, i, objList, lsList);

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
    Vector3D direct_illumination = Vector3D();

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
            reflectance = shape->getMaterial().getReflectance(n, wo, wi);

            //Direct illumination equation
            direct_illumination += incident_light * reflectance * NdotL;
        }
    }

    //Global Illumination
    Vector3D global_illumination = computeGlobalIllumination(r, i, objList, lsList);
  
    //Output
    return direct_illumination + global_illumination;
}

Vector3D ExplicitGlobalShader::computeMirror(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const
{
    //Declare variables
    const Vector3D n = i.normal.normalized();
    const Vector3D p = i.itsPoint;
    const Vector3D wo = -r.d.normalized();

    //Compute perfect reflection ray
    const Vector3D wr = i.shape->getMaterial().getPerfectReflection(n, wo);
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
    Vector3D wo = -r.d.normalized();
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
            //Compute perfect specular direction
            const Vector3D wr = i.shape->getMaterial().getPerfectReflection(n, wo);
            const Ray reflectionRay = Ray(p, wr, r.depth);
            return computeColor(reflectionRay, objList, lsList);
        }
    }

}

Vector3D ExplicitGlobalShader::computeGlobalIllumination(const Ray& r, const Intersection& i, const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList) const
{
    //Declare variables
    const Shape* shape = i.shape;
    const Vector3D n = i.normal.normalized();
    const Vector3D p = i.itsPoint;
    const Vector3D wo = -r.d.normalized();
    const Vector3D kd = shape->getMaterial().getDiffuseCoefficient();

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
                    Vector3D sampling_direction = sampler->getSample(n).normalized();
                    const Ray sampling_ray = Ray(p, sampling_direction, r.depth + 1);

                    //Incoming radiance
                    Vector3D incoming_radiance = computeColor(sampling_ray, objList, lsList);

                    //Reflectance
                    Vector3D reflectance = shape->getMaterial().getReflectance(n, wo, sampling_direction);

                    //NdotS
                    const double NdotS = dot(n, sampling_direction);

                    //Outgoing radiance
                    outgoing_radiance += incoming_radiance * reflectance;
                }

                //Output
                outgoing_radiance *= 1.0 / (2.0 * M_PI * nSamples);
                return outgoing_radiance;
            }
            else
            {
                //Approximation
               return kd * ambient_term;
            }
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
                    Vector3D sampling_direction = sampler->getSample(n).normalized();
                    const Ray sampling_ray = Ray(p, sampling_direction, r.depth + 1);

                    //Incoming radiance
                    Vector3D incoming_radiance = computeColor(sampling_ray, objList, lsList);

                    //Reflectance
                    Vector3D reflectance = shape->getMaterial().getReflectance(n, wo, sampling_direction);

                    //NdotS
                    const double NdotS = dot(n, sampling_direction);

                    //Outgoing radiance
                    outgoing_radiance += incoming_radiance * reflectance;

                }
                //Output
                outgoing_radiance *= 1 / (2 * M_PI * nSamples);
                return outgoing_radiance;

            }
            else if (r.depth == maxDepth)
            {
                //Approximation
                return kd * ambient_term;
            }
            else
            {
                //Declare variables
                Vector3D outgoing_radiance = Vector3D();

                //Normal direction
                const Ray normal_ray = Ray(p, n, r.depth + 1);
                outgoing_radiance += computeColor(normal_ray, objList, lsList) * shape->getMaterial().getReflectance(n, wo, n);

                //Perfect Reflection Direction
                const Vector3D wr = shape->getMaterial().getPerfectReflection(n, wo).normalized();
                const Ray perfect_reflection_ray = Ray(p, wr, r.depth + 1);
                const double NdotR = dot(n, wr);
                outgoing_radiance += computeColor(perfect_reflection_ray, objList, lsList) * shape->getMaterial().getReflectance(n, wo, wr);

                //Outgoing radiance
                outgoing_radiance *= 1 / (4 * M_PI);
                return outgoing_radiance;

            }
        }
    }
}

#include <iostream>
#include <stdlib.h> /* srand, rand */
#include <vector>
#include <algorithm>

#include "core/film.h"
#include "core/matrix4x4.h"
#include "core/ray.h"
#include "core/utils.h"

#include "shapes/sphere.h"
#include "shapes/infiniteplan.h"

#include "cameras/ortographic.h"
#include "cameras/perspective.h"

#include "shaders/intersectionshader.h"
#include "shaders/depthshader.h"
#include "shaders/normalshader.h"
#include "shaders/directshader.h"

using namespace std;

void buildSceneSphere(Camera*& cam, Film*& film, std::vector<Shape*>*& objectsList, std::vector<PointLightSource>*& lightSourceList)
{
    /* **************************** */
      /* Declare and place the camera */
      /* **************************** */
      // By default, this gives an ID transform
      //  which means that the camera is located at (0, 0, 0)
      //  and looking at the "+z" direction
    Matrix4x4 cameraToWorld;
    double fovDegrees = 60;
    double fovRadians = Utils::degreesToRadians(fovDegrees);
    cam = new PerspectiveCamera(cameraToWorld, fovRadians, *film);

    /* ************************** */
    /* DEFINE YOUR MATERIALS HERE */
    /* ************************** */
    Material* material1 = new Phong (Vector3D(0.2, 0.7, 0.3), Vector3D(0.2, 0.6, 0.2), 50.0);
    Material* material2 = new Phong(Vector3D(0.5922, 0.0166, 0.1000), Vector3D(0.5974, 0.2084, 0.2084), 70.0);
    Material* material3 = new Phong(Vector3D(0.65, 0.34, 0.7), Vector3D(0.85, 0.36, 0.28), 35.0);
    
    /* ******* */
    /* Objects */
    /* ******* */

    // Create a heterogeneous list of objects of type shape (some might be triangles, other spheres, plans, etc)
    objectsList = new std::vector<Shape*>;

    // Define and place a sphere
    Matrix4x4* sphereTransform1 = new Matrix4x4();
    *sphereTransform1 = sphereTransform1->translate(Vector3D(-1.0, -0.5, 2 * std::sqrt(2.0)));
    Shape* s1 = new Sphere(0.25, sphereTransform1, material1);

    // Define and place a sphere
    Matrix4x4* sphereTransform2 = new Matrix4x4();
    *sphereTransform2 = sphereTransform2->translate(Vector3D(1.0, 0.0, 6));
    Shape* s2 = new Sphere(1, sphereTransform2, material2);

    // Define and place a sphere
    Matrix4x4* sphereTransform3 = new Matrix4x4();
    *sphereTransform3 = sphereTransform3->translate(Vector3D(0.3, -0.75, 3.5));
    Shape* s3 = new Sphere(0.25, sphereTransform3, material3);

    // Store the objects in the object list
    objectsList->push_back(s1);
    objectsList->push_back(s2);
    objectsList->push_back(s3);

    /* ****** */
    /* Lights */
    /* ****** */

    //Initialize lights list
    lightSourceList = new std::vector<PointLightSource>;
    
    // ADD YOUR LIGHT SOURCES HERE
    PointLightSource light1 = PointLightSource(Vector3D(1.0, 1.5, 3.5), Vector3D(5.0, 2.5, 1.0));
    PointLightSource light2 = PointLightSource(Vector3D(2.0, 0.0, 3.0), Vector3D(2.5, 5.0, 1.0));
    PointLightSource light3 = PointLightSource(Vector3D(-1.5, -1.5, 2.0), Vector3D(1.0, 2.5, 5.0));    
    
    // DO NOT FORGET TO STORE THE LIGHT SOURCES IN THE "lightSourceList"
    lightSourceList->push_back(light1);
    lightSourceList->push_back(light2);
    lightSourceList->push_back(light3);

    //Check whether lights are inside the shapes
    for (int i = 0; i < lightSourceList->size(); i++)
    {
        PointLightSource& current_light = lightSourceList->at(i);
        for (int j = 0; j < objectsList->size(); j++)
        {
            Shape* current_shape = objectsList->at(j);
            bool light_inside = current_shape->isPointInside(current_light.getPosition());
            if (light_inside) cout << "Light " << i << " is inside the shape " << j << endl;
        }
    }
    
}

void raytrace(Camera* &cam, Shader* &shader, Film* &film,
              std::vector<Shape*>* &objectsList, std::vector<PointLightSource>* &lightSourceList)
{
    unsigned int sizeBar = 40;

    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    // Main raytracing loop
    // Out-most loop invariant: we have rendered lin lines
    for(size_t lin=0; lin<resY; lin++)
    {
        // Show progression
        if ( lin%(resY/sizeBar) == 0)
            std::cout << ".";

        // Inner loop invariant: we have rendered col columns
        for(size_t col=0; col<resX; col++)
        {
            // Compute the pixel position in NDC
            double x = (double)(col + 0.5) / resX;
            double y = (double)(lin + 0.5) / resY;

            // Generate the camera ray
            Ray cameraRay = cam->generateRay(x, y);

            // Compute ray color according to the used shader
            Vector3D pixelColor = shader->computeColor( cameraRay, *objectsList, *lightSourceList );

            // Store the pixel color
            film->setPixelValue(col, lin, pixelColor);
        }
    }
}


//------------TASK 1---------------------//
void PaintImage(Film* film)
{
    unsigned int sizeBar = 40;

    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    // Main Image Loop
    for (size_t row = 0; row < resY; row++)
    {
        // Show progression
        if (row % (resY / sizeBar) == 0)
            std::cout << ".";

        for (size_t col = 0; col < resX; col++)
        {
       
            Vector3D position_color = Vector3D(col / (double)resX, row / (double)resY, 0);
            //position_color.print();

            //Vector3D random_color = Vector3D((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX);            
            film->setPixelValue(col,row, position_color);
           
        }
    }
}

int main()
{
    std::string separator     = "\n----------------------------------------------\n";
    std::string separatorStar = "\n**********************************************\n";
    std::cout << separator << "RT-ACG - Ray Tracer for \"Advanced Computer Graphics\"" << separator << std::endl;

    // Create an empty film
    Film *film;
    film = new Film(720, 512);


    // Declare the shader attributes
    Vector3D bgColor(0.0, 0.0, 0.0); // Background color (for rays which do not intersect anything)
    Vector3D intersectionColor(1,0,0);
    Vector3D depthColor(0, 1, 0.4);
    double maxDistance = 8.0;
    
    //Get the proper shader
    Shader* intersection_shader = new IntersectionShader (intersectionColor, bgColor);
    Shader* depth_shader = new DepthShader(depthColor, maxDistance, bgColor);
    Shader* normal_shader = new NormalShader(bgColor);
    Shader* direct_shader = new DirectShader(bgColor);

    // Build the scene---------------------------------------------------------
    // 
    // Declare pointers to all the variables which describe the scene
    Camera* cam;
    std::vector<Shape*>* objectsList;
    std::vector<PointLightSource>* lightSourceList;
    //Create Scene Geometry and Illumiantion
    buildSceneSphere(cam, film, objectsList, lightSourceList);

    //---------------------------------------------------------------------------

    //Paint Image ONLY TASK 1
    //PaintImage(film);

    // Launch some rays!
    
    // TASK 2
    //raytrace(cam, intersection_shader, film, objectsList, lightSourceList);

    //TASK 3
    //raytrace(cam, depth_shader, film, objectsList, lightSourceList);

    //TASK 4
    //raytrace(cam, normal_shader, film, objectsList, lightSourceList);

    //TASK 5
    raytrace(cam, direct_shader, film, objectsList, lightSourceList);

    // Save the final result to file
    std::cout << "\n\nSaving the result to file output.bmp\n" << std::endl;
    film->save();

    std::cout << "\n\n" << std::endl;
    return 0;
}

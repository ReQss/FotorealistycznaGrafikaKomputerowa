#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include "../include/Vector3.h"
#include "../include/Geometry.h"
#include "../include/Image.h"
#include "../include/Camera.h"
#include "../include/Material.h"
#include "../include/Light.h"

using namespace std;

optional<HitRecord> closestHit(const Ray& ray,
                               const vector<Sphere>& spheres,
                               const vector<Plane>&  planes)
{
    optional<HitRecord> best;

    for (const auto& sphere : spheres)
    {
        auto hit = sphere.intersects(ray);
        if (hit && (!best || hit->t < best->t))
            best = hit;
    }
    for (const auto& plane : planes)
    {
        auto hit = plane.intersects(ray);
        if (hit && (!best || hit->t < best->t))
            best = hit;
    }
    return best;
}

Color phong(const HitRecord& rec,
            const Ray& ray,
            const PointLight& light,
            const vector<Sphere>& spheres,
            const vector<Plane>&  planes)
{
    Color ambient = rec.material.Ka * light.intensity;

    if (light.isInShadow(rec, spheres, planes))
        return ambient;

    Color diffuse = light.getDiffuse(ray.origin, rec);

    Color specular = light.getSpecular(ray.origin, rec);

    return ambient + diffuse + specular;
}

Vector3 reflect(const Vector3& I, const Vector3& N) 
{
    return I - N * (2.0 * (N * I));
}

std::optional<Vector3> refract(const Vector3& I, const Vector3& N, double ior) 
{
    double cosi = I * N;
    double etai = 1.0; 
    double etat = ior;
    Vector3 n = N;
    
    if (cosi < 0) { 
        cosi = -cosi; 
    } else { 
        std::swap(etai, etat); 
        n = N * -1.0; 
    }
    
    double eta = etai / etat;
    double k = 1.0 - eta * eta * (1.0 - cosi * cosi);
    
    if (k < 0.0) {
        return std::nullopt;
    } else {
        return I * eta + n * (eta * cosi - sqrt(k));
    }
}

Color trace(const Ray& ray, 
            const vector<Sphere>& spheres, 
            const vector<Plane>& planes, 
            const PointLight& light,
            int depth) 
{
    if (depth <= 0) {
        return Color(0, 0, 0);
    }

    auto hit = closestHit(ray, spheres, planes);
    if (!hit) {
        return Color(0, 0, 0);
    }

    HitRecord rec = *hit;
    Color finalColor(0, 0, 0);

    if (rec.material.isMirror) 
    {
        Vector3 R = reflect(ray.direction, rec.normal).normalize();
        Ray reflectRay(rec.point + rec.normal * 1e-4, R);
        finalColor = trace(reflectRay, spheres, planes, light, depth - 1);
    } 
    else if (rec.material.isRefractive) 
    {
        auto refractedDir = refract(ray.direction, rec.normal, rec.material.ior);
        if (refractedDir) {
            Vector3 offsetNormal = (ray.direction * rec.normal < 0) ? rec.normal : rec.normal * -1.0;
            Ray refractRay(rec.point - offsetNormal * 1e-4, refractedDir->normalize());
            
            finalColor = trace(refractRay, spheres, planes, light, depth - 1);
        } else {
            Vector3 R = reflect(ray.direction, rec.normal).normalize();
            Ray reflectRay(rec.point + rec.normal * 1e-4, R);
            finalColor = trace(reflectRay, spheres, planes, light, depth - 1);
        }
    } 
    else 
    {
        finalColor = phong(rec, ray, light, spheres, planes);
    }

    return finalColor;
}

double colorDifference(const Color& c1, const Color& c2)
{
    return max({fabs(c1.r-c2.r), fabs(c1.g-c2.g), fabs(c1.b-c2.b)});
}

Color adaptiveSample(double x, double y, double size, int depth, int maxDepth,
                     const Camera& camera, int width, int height,
                     const vector<Sphere>& spheres, const vector<Plane>& planes,
                     const PointLight& light)
{
    Ray   centerRay   = camera.generateRay(x, y, width, height);
    Color centerColor = trace(centerRay, spheres, planes, light, 4);

    if (depth >= maxDepth) return centerColor;

    double q = size / 4.0;
    Color c1 = trace(camera.generateRay(x-q, y-q, width, height), spheres, planes, light, 4);
    Color c2 = trace(camera.generateRay(x+q, y-q, width, height), spheres, planes, light, 4);
    Color c3 = trace(camera.generateRay(x-q, y+q, width, height), spheres, planes, light, 4);
    Color c4 = trace(camera.generateRay(x+q, y+q, width, height), spheres, planes, light, 4);

    double threshold = 0.05;
    bool needsSub = colorDifference(centerColor, c1) > threshold ||
                    colorDifference(centerColor, c2) > threshold ||
                    colorDifference(centerColor, c3) > threshold ||
                    colorDifference(centerColor, c4) > threshold;

    if (needsSub)
    {
        double h = size / 2.0;
        c1 = adaptiveSample(x-q, y-q, h, depth+1, maxDepth, camera, width, height, spheres, planes, light);
        c2 = adaptiveSample(x+q, y-q, h, depth+1, maxDepth, camera, width, height, spheres, planes, light);
        c3 = adaptiveSample(x-q, y+q, h, depth+1, maxDepth, camera, width, height, spheres, planes, light);
        c4 = adaptiveSample(x+q, y+q, h, depth+1, maxDepth, camera, width, height, spheres, planes, light);
    }

    return (c1 + c2 + c3 + c4) * 0.25;
}

void renderScene(const Camera& camera, int width, int height,
                 const string& filename, int maxDepth,
                 const vector<Sphere>& spheres, const vector<Plane>& planes,
                 const PointLight& light)
{
    Image img(width, height);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            Color pixelColor;
            if (maxDepth == 0)
            {
                Ray ray = camera.generateRay(x+0.5, y+0.5, width, height);
                pixelColor = trace(ray, spheres, planes, light, 4); 
            }
            else
            {
                pixelColor = adaptiveSample(x+0.5, y+0.5, 1.0, 0, maxDepth,
                                            camera, width, height, spheres, planes, light);
            }
            img.setPixel(x, y, pixelColor);
        }
    img.savePPM(filename);
}

int main()
{
    Vector3 a(1, 2, 3), b(4, 5, 6);
    Vector3 sum1 = a + b, sum2 = b + a;
    cout << "a = " << a << "\nb = " << b << endl;
    cout << "a + b = " << sum1 << "\nb + a = " << sum2 << endl;
    cout << (sum1 == sum2 ? "Dodawanie przemienne." : "Dodawanie NIE jest przemienne.") << endl;

    Vector3 v1(0,3,0), v2(5,5,0);
    cout << "Kat miedzy wektorami: " << v1.angle(v2) << " stopni" << endl;

    Vector3 u(4,5,1), v(4,1,3);
    Vector3 perp = u.cross(v);
    cout << "Wektor prostopadly: " << perp << endl;
    cout << "Znormalizowany:     " << perp.normalize() << endl;

    Sphere S(Vector3(0,0,0), 10.0);
    Ray R1(Vector3(0,0,-20), Vector3(0,0,1));
    Ray R2(Vector3(0,0,-20), Vector3(0,1,0));
    Ray R3(Vector3(10,0,-10), Vector3(0,0,1));

    auto hit1 = S.intersects(R1);
    cout << (hit1 ? "R1 trafil: " : "R1 NIE trafil"); if(hit1) cout << hit1->point; cout << endl;
    auto hit2 = S.intersects(R2);
    cout << (hit2 ? "R2 trafil: " : "R2 NIE trafil"); if(hit2) cout << hit2->point; cout << endl;
    auto hit3 = S.intersects(R3);
    cout << (hit3 ? "R3 trafil: " : "R3 NIE trafil"); if(hit3) cout << hit3->point; cout << endl;

    Plane P(Vector3(0,0,0), Vector3(0,1,1));
    auto hit4 = P.intersects(R2);
    cout << (hit4 ? "R2 trafil plaszczyzne: " : "R2 NIE trafil plaszczyzny"); if(hit4) cout << hit4->point; cout << endl;

    Vector3 A(0,0,0), B(1,0,0), C(0,1,0);
    cout << "Przypadek 1: " << (isLineIntersectingTriangle(Vector3(-1,0.5,0),Vector3(1,0.5,0),A,B,C)?"True":"False") << endl;
    cout << "Przypadek 2: " << (isLineIntersectingTriangle(Vector3(2,-1,0), Vector3(2,2,0),  A,B,C)?"True":"False") << endl;
    cout << "Przypadek 3: " << (isLineIntersectingTriangle(Vector3(0,0,-1), Vector3(0,0,1),  A,B,C)?"True":"False") << endl;

    // LAB 5 - Rekursywny ray tracing z adaptacyjnym próbkowaniem
   
    int width = 600, height = 600;

    Material matWhite(Color(0.1, 0.1, 0.1), Color(0.8, 0.8, 0.8), Color(0.0, 0.0, 0.0), 1.0);
    Material matRed(  Color(0.1, 0.0, 0.0), Color(0.8, 0.1, 0.1), Color(0.0, 0.0, 0.0), 1.0);
    Material matGreen(Color(0.0, 0.1, 0.0), Color(0.1, 0.8, 0.1), Color(0.0, 0.0, 0.0), 1.0);

    Material matMirror(Color(0,0,0), Color(0,0,0), Color(0,0,0), 0, true, false, 1.0);
    Material matGlass(Color(0,0,0), Color(0,0,0), Color(0,0,0), 0, false, true, 1.52);

    vector<Plane> planes = {
        Plane(Vector3( 0, -3,  0), Vector3( 0,  1,  0), matWhite),
        Plane(Vector3( 0,  3,  0), Vector3( 0, -1,  0), matWhite),
        Plane(Vector3( 0,  0, -6), Vector3( 0,  0,  1), matWhite),
        Plane(Vector3(-3,  0,  0), Vector3( 1,  0,  0), matRed),
        Plane(Vector3( 3,  0,  0), Vector3(-1,  0,  0), matGreen)
    };

    vector<Sphere> spheres = {
        Sphere(Vector3(-1.3, -1.5, -4.5), 1.5, matMirror),
        Sphere(Vector3( 1.3, -1.5, -2.5), 1.5, matGlass)
    };

    PointLight light(
        Vector3(0.0, 2.9, -3.0),
        Color(1.0, 1.0, 1.0), 
        0.5, 0.08, 0.005
    );

    PerspectiveCamera perspCam(Vector3(0.0, 0.0, 2.9), Vector3(0.0, 0.0, -1.0), Vector3(0.0, 1.0, 0.0), 80.0);

    int maxDepth = 4; 
    
    renderScene(perspCam, width, height, "cornell_box.ppm", 2, spheres, planes, light);

    return 0;
}
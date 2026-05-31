#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include "Vector3.h"
#include "Geometry.h"
#include "Image.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"

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

Color trace(const Ray& ray,
            const vector<Sphere>& spheres,
            const vector<Plane>&  planes,
            const PointLight& light)
{
    auto hit = closestHit(ray, spheres, planes);

    if (hit)
    {
        Color c = phong(*hit, ray, light, spheres, planes);
        c.r = clamp(c.r, 0.0, 1.0);
        c.g = clamp(c.g, 0.0, 1.0);
        c.b = clamp(c.b, 0.0, 1.0);
        return c;
    }

    Color finalColor(0.05, 0.05, 0.1);
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
    Color centerColor = trace(centerRay, spheres, planes, light);

    if (depth >= maxDepth) return centerColor;

    double q = size / 4.0;
    Color c1 = trace(camera.generateRay(x-q, y-q, width, height), spheres, planes, light);
    Color c2 = trace(camera.generateRay(x+q, y-q, width, height), spheres, planes, light);
    Color c3 = trace(camera.generateRay(x-q, y+q, width, height), spheres, planes, light);
    Color c4 = trace(camera.generateRay(x+q, y+q, width, height), spheres, planes, light);

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
                pixelColor = trace(ray, spheres, planes, light);
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

    // LAB 4 – Phong + cien
   
    int width = 600, height = 600;

    Material matBlue(
        Color(0.0, 0.0, 0.1),
        Color(0.1, 0.3, 0.9),
        Color(0.0, 0.0, 0.0),
        200
    );
    Material matRed(
        Color(0.1, 0.0, 0.0),
        Color(0.9, 0.2, 0.2),
        Color(1.0, 1.0, 1.0),
        32.0
    );

    Material matFloor(
        Color(0.05, 0.05, 0.05),
        Color(0.6,  0.6,  0.6),
        Color(0.1,  0.1,  0.1),
        8.0
    );

    vector<Sphere> spheres = {
        Sphere(Vector3( 0.0, 0.0, -4.0), 1.2, matBlue),
        Sphere(Vector3(5, 0.0, -5.5), 0.8, matRed)
    };

    vector<Plane> planes = {
        Plane(Vector3(0, -1.2, 0), Vector3(0, 1, 0), matFloor)
    };

    PointLight light(
        Vector3(0.0, 1.0, -2.0),
        Color(1.0, 1.0, 1.0),
        1.0, 0.04, 0.002
    );

    PerspectiveCamera perspCam(Vector3(0, 1.5, 2.0), Vector3(0, -0.3, -1), Vector3(0, 1, 0), 70.0);
    OrthographicCamera orthoCam(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0), 5.0);

    renderScene(perspCam, width, height, "shadow_perspective.ppm",  0, spheres, planes, light);
    renderScene(perspCam, width, height, "shadow_perspective_aa.ppm", 2, spheres, planes, light);
    renderScene(orthoCam, width, height, "shadow_ortho.ppm",         0, spheres, planes, light);

    return 0;
}
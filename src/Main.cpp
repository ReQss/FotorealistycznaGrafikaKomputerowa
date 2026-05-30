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

// -------------------------------------------------------
// Pomocnicze: szuka najblizszego trafienia wsrod sfer
// -------------------------------------------------------
optional<HitRecord> closestHit(const Ray& ray,
                               const vector<Sphere>& spheres)
{
    optional<HitRecord> best;
    for (const auto& sphere : spheres)
    {
        auto hit = sphere.intersects(ray);
        if (hit && (!best || hit->t < best->t))
            best = hit;
    }
    return best;
}

// -------------------------------------------------------
// Model oswietlenia Phonga dla jednego zrodla punktowego
// -------------------------------------------------------
Color phong(const HitRecord& rec,
            const Ray& ray,
            const PointLight& light,
            const vector<Sphere>& spheres)
{
    const Material& mat = rec.material;

    // --- kierunek do swiatla i odleglosc ---
    Vector3 L    = light.directionFrom(rec.point);
    double  dist = light.distanceFrom(rec.point);

    // --- promien cienia ---
    Ray shadowRay(rec.point + L * 1e-4, L);
    bool inShadow = false;
    for (const auto& sphere : spheres)
    {
        auto hit = sphere.intersects(shadowRay);
        // liczymy tylko przeszkody blizsze niz samo zrodlo swiatla
        if (hit && hit->t < dist)
        {
            inShadow = true;
            break;
        }
    }

    // --- skladnik ambient (zawsze) ---
    Color ambient = mat.Ka * light.intensity;

    if (inShadow)
        return ambient;

    // --- zanikanie ---
    double atten = light.attenuationAt(dist);

    // --- diffuse (Lambertian) ---
    double diff    = max(0.0, rec.normal * L);
    Color  diffuse = mat.Kd * light.intensity * (diff * atten);

    // --- specular (Phong) ---
    Vector3 V        = (ray.origin - rec.point).normalize();
    Vector3 R        = (rec.normal * (2.0 * (rec.normal * L)) - L).normalize();
    double  spec     = pow(max(0.0, V * R), mat.shininess);
    Color   specular = mat.Ks * light.intensity * (spec * atten);

    return ambient + diffuse + specular;
}

// -------------------------------------------------------
// Funkcja trace
// -------------------------------------------------------
Color trace(const Ray& ray,
            const vector<Sphere>& spheres,
            const PointLight& light)
{
    auto hit = closestHit(ray, spheres);

    if (hit)
    {
        Color c = phong(*hit, ray, light, spheres);
        c.r = clamp(c.r, 0.0, 1.0);
        c.g = clamp(c.g, 0.0, 1.0);
        c.b = clamp(c.b, 0.0, 1.0);
        return c;
    }

    // --- tlo: siatka kolorowych pasow ---
    Color finalColor(0, 0, 0);
    double tz = ray.direction.getZ();
    if (fabs(tz) > 1e-6)
    {
        double t_bg = (-5.0 - ray.origin.getZ()) / tz;
        if (t_bg > 0.0)
        {
            Vector3 bgHit = ray.origin + ray.direction * t_bg;
            double x = bgHit.getX();
            double y = bgHit.getY();
            int col = clamp((int)floor(x + 3.0), 0, 5);
            int row = clamp((int)floor(3.0 - y), 0, 5);
            double t = row / 5.0;

            switch (col)
            {
                case 5: finalColor = Color(t, 0, 0);    break;
                case 4: finalColor = Color(0, t, 0);    break;
                case 3: finalColor = Color(0, 0, t);    break;
                case 2: finalColor = Color(1.0, 0, t);  break;
                case 1: finalColor = Color(0, 1.0, t);  break;
                case 0: finalColor = Color(1.0, 1.0, t);break;
            }
        }
    }
    return finalColor;
}

// -------------------------------------------------------
// Adaptacyjny supersampling
// -------------------------------------------------------
double colorDifference(const Color& c1, const Color& c2)
{
    return max({fabs(c1.r - c2.r), fabs(c1.g - c2.g), fabs(c1.b - c2.b)});
}

Color adaptiveSample(double x, double y, double size, int depth, int maxDepth,
                     const Camera& camera, int width, int height,
                     const vector<Sphere>& spheres, const PointLight& light)
{
    Ray   centerRay   = camera.generateRay(x, y, width, height);
    Color centerColor = trace(centerRay, spheres, light);

    if (depth >= maxDepth) return centerColor;

    double q = size / 4.0;
    Ray r1 = camera.generateRay(x - q, y - q, width, height);
    Ray r2 = camera.generateRay(x + q, y - q, width, height);
    Ray r3 = camera.generateRay(x - q, y + q, width, height);
    Ray r4 = camera.generateRay(x + q, y + q, width, height);

    Color c1 = trace(r1, spheres, light);
    Color c2 = trace(r2, spheres, light);
    Color c3 = trace(r3, spheres, light);
    Color c4 = trace(r4, spheres, light);

    double threshold = 0.05;
    bool needsSub = colorDifference(centerColor, c1) > threshold ||
                    colorDifference(centerColor, c2) > threshold ||
                    colorDifference(centerColor, c3) > threshold ||
                    colorDifference(centerColor, c4) > threshold;

    if (needsSub)
    {
        double h = size / 2.0;
        c1 = adaptiveSample(x - q, y - q, h, depth + 1, maxDepth, camera, width, height, spheres, light);
        c2 = adaptiveSample(x + q, y - q, h, depth + 1, maxDepth, camera, width, height, spheres, light);
        c3 = adaptiveSample(x - q, y + q, h, depth + 1, maxDepth, camera, width, height, spheres, light);
        c4 = adaptiveSample(x + q, y + q, h, depth + 1, maxDepth, camera, width, height, spheres, light);
    }

    return (c1 + c2 + c3 + c4) * 0.25;
}

// -------------------------------------------------------
// Render
// -------------------------------------------------------
void renderScene(const Camera& camera, int width, int height,
                 const string& filename, int maxDepth,
                 const vector<Sphere>& spheres, const PointLight& light)
{
    Image img(width, height);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            Color pixelColor;
            if (maxDepth == 0)
            {
                Ray ray = camera.generateRay(x + 0.5, y + 0.5, width, height);
                pixelColor = trace(ray, spheres, light);
            }
            else
            {
                pixelColor = adaptiveSample(x + 0.5, y + 0.5, 1.0, 0, maxDepth,
                                            camera, width, height, spheres, light);
            }
            img.setPixel(x, y, pixelColor);
        }
    img.savePPM(filename);
}

// -------------------------------------------------------
// main
// -------------------------------------------------------
int main()
{
    // ===========================
    // LAB 1 – testy wektorow itp.
    // ===========================
    Vector3 a(1, 2, 3);
    Vector3 b(4, 5, 6);

    Vector3 sum1 = a + b;
    Vector3 sum2 = b + a;

    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << "a + b = " << sum1 << endl;
    cout << "b + a = " << sum2 << endl;

    if (sum1 == sum2)
        cout << "Dodawanie wektorow jest przemienne." << endl;
    else
        cout << "Dodawanie wektorow NIE jest przemienne." << endl;

    Vector3 v1(0, 3, 0), v2(5, 5, 0);
    cout << "Kat miedzy wektorami: " << v1.angle(v2) << " stopni" << endl;

    Vector3 u(4, 5, 1), v(4, 1, 3);
    Vector3 perp = u.cross(v);
    cout << "Wektor prostopadly do [4,5,1] i [4,1,3]: " << perp << endl;
    cout << "Znormalizowany: " << perp.normalize() << endl;

    // Sfera i promienie
    Sphere S(Vector3(0,0,0), 10.0);
    Ray R1(Vector3(0,0,-20), Vector3(0,0,1));
    Ray R2(Vector3(0,0,-20), Vector3(0,1,0));

    auto hit1 = S.intersects(R1);
    cout << (hit1 ? ("R1 trafil w sfere S: ") : "R1 NIE trafil w sfere S");
    if (hit1) cout << hit1->point;
    cout << endl;

    auto hit2 = S.intersects(R2);
    cout << (hit2 ? "R2 trafil w sfere S: " : "R2 NIE trafil w sfere S");
    if (hit2) cout << hit2->point;
    cout << endl;

    Ray R3(Vector3(10, 0, -10), Vector3(0, 0, 1));
    auto hit3 = S.intersects(R3);
    cout << (hit3 ? "R3 trafil w sfere S: " : "R3 NIE trafil w sfere S");
    if (hit3) cout << hit3->point;
    cout << endl;

    Plane P(Vector3(0,0,0), Vector3(0,1,1));
    auto hit4 = P.intersects(R2);
    cout << (hit4 ? "R2 trafil w plaszczyzne P: " : "R2 NIE trafil w plaszczyzne P");
    if (hit4) cout << hit4->point;
    cout << endl;

    // Trojkat
    Vector3 A(0,0,0), B(1,0,0), C(0,1,0);
    cout << "Przypadek 1: " << (isLineIntersectingTriangle(Vector3(-1,0.5,0), Vector3(1,0.5,0), A,B,C) ? "True":"False") << endl;
    cout << "Przypadek 2: " << (isLineIntersectingTriangle(Vector3(2,-1,0),  Vector3(2,2,0),   A,B,C) ? "True":"False") << endl;
    cout << "Przypadek 3: " << (isLineIntersectingTriangle(Vector3(0,0,-1),  Vector3(0,0,1),   A,B,C) ? "True":"False") << endl;

    // ===========================
    // LAB 4 – Phong + cien
    // ===========================
    int width = 600, height = 600;

    // Materialy
    Material matBlue(
        Color(0.0, 0.0, 0.1),   // Ka
        Color(0.1, 0.3, 0.9),   // Kd
        Color(1.0, 1.0, 1.0),   // Ks
        64.0                     // shininess
    );
    Material matRed(
        Color(0.1, 0.0, 0.0),
        Color(0.9, 0.2, 0.2),
        Color(1.0, 1.0, 1.0),
        32.0
    );

    vector<Sphere> spheres = {
        Sphere(Vector3(0.0,  0.0, -4.0), 1.2, matBlue),
        Sphere(Vector3(-1.5, 0.0, -5.5), 0.8, matRed)
    };

    // Zrodlo punktowe: pozycja, kolor/natezenie, wspolczynniki zanikania
    PointLight light(
        Vector3(3.0, 5.0, -2.0),   // pozycja
        Color(1.0, 1.0, 1.0),       // biale swiatlo
        1.0, 0.05, 0.005            // const, linear, quadratic
    );

    OrthographicCamera orthoCam(Vector3(0,0,0), Vector3(0,0,-1), Vector3(0,1,0), 5.0);
    PerspectiveCamera  perspCam(Vector3(0,0.5,0), Vector3(0,-0.1,-1), Vector3(0,1,0), 90.0);

    renderScene(orthoCam, width, height, "orthographic_phong.ppm",    0, spheres, light);
    renderScene(perspCam, width, height, "perspective_phong_1spp.ppm", 0, spheres, light);
    renderScene(perspCam, width, height, "perspective_phong_aa.ppm",   2, spheres, light);

    return 0;
}
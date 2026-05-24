#include <iostream>
#include "../include/Vector3.h"
#include "../include/Geometry.h"
#include "../include/Image.h"
#include "../include/Camera.h"

using namespace std;

double colorDifference(const Color& c1, const Color& c2) {
    return std::max({std::abs(c1.r - c2.r), std::abs(c1.g - c2.g), std::abs(c1.b - c2.b)});
}

Color trace(const Ray& ray, const Sphere& sphere, const Plane& plane) {
    auto hitSphere = sphere.intersects(ray);
    auto hitPlane = plane.intersects(ray);

    double minDistance = 1e9; 
    
    Color finalColor(0.2, 0.2, 0.3); 

    Vector3 lightDir = Vector3(1, 1, -1).normalize();

    if (hitSphere) {
        double dist = (*hitSphere - ray.origin).length();
        
        if (dist < minDistance) {
            minDistance = dist;
            
            Vector3 normal = (*hitSphere - sphere.center).normalize();
            
            double intensity = std::max(0.0, normal * lightDir);
            
            Color sphereColor(0.8, 0.2, 0.2); 
            
            finalColor = sphereColor * intensity;
        }
    }

    if (hitPlane) {
        double dist = (*hitPlane - ray.origin).length();
        
        if (dist < minDistance) {
            minDistance = dist;
            
            Vector3 normal = plane.normal;
            
            double intensity = std::max(0.0, normal * lightDir);
            
            Color planeColor(0.5, 0.5, 0.5); 
            
            finalColor = planeColor * intensity;
        }
    }

    return finalColor;
}

Color adaptiveSample(double x, double y, double size, int depth, int maxDepth, const Camera& camera, int width, int height, const Sphere& sphere, const Plane& plane) {
    Ray centerRay = camera.generateRay(x, y, width, height);
    Color centerColor = trace(centerRay, sphere, plane);

    if (depth >= maxDepth) {
        return centerColor;
    }

    double quarterSize = size / 4.0;
    Ray r1 = camera.generateRay(x - quarterSize, y - quarterSize, width, height);
    Ray r2 = camera.generateRay(x + quarterSize, y - quarterSize, width, height);
    Ray r3 = camera.generateRay(x - quarterSize, y + quarterSize, width, height);
    Ray r4 = camera.generateRay(x + quarterSize, y + quarterSize, width, height);

    Color c1 = trace(r1, sphere, plane);
    Color c2 = trace(r2, sphere, plane);
    Color c3 = trace(r3, sphere, plane);
    Color c4 = trace(r4, sphere, plane);

    double threshold = 0.1;
    
    bool needsSubdivision = 
        colorDifference(centerColor, c1) > threshold ||
        colorDifference(centerColor, c2) > threshold ||
        colorDifference(centerColor, c3) > threshold ||
        colorDifference(centerColor, c4) > threshold;

    if (needsSubdivision) {
        double halfSize = size / 2.0;
        c1 = adaptiveSample(x - quarterSize, y - quarterSize, halfSize, depth + 1, maxDepth, camera, width, height, sphere, plane);
        c2 = adaptiveSample(x + quarterSize, y - quarterSize, halfSize, depth + 1, maxDepth, camera, width, height, sphere, plane);
        c3 = adaptiveSample(x - quarterSize, y + quarterSize, halfSize, depth + 1, maxDepth, camera, width, height, sphere, plane);
        c4 = adaptiveSample(x + quarterSize, y + quarterSize, halfSize, depth + 1, maxDepth, camera, width, height, sphere, plane);
    }

    return (c1 + c2 + c3 + c4) * 0.25;
}

int main()
{
    // -------------------------------
    // LAB 1
    // -------------------------------
    Vector3 a(1, 2, 3);
    Vector3 b(4, 5, 6);

    Vector3 sum1 = a + b;
    Vector3 sum2 = b + a;

    cout << "a = " << a << endl;
    cout << "b = " << b << endl;

    cout << "a + b = " << sum1 << endl;
    cout << "b + a = " << sum2 << endl;

    /* 2. Sprawdź czy prawidłowo działa przemienność dodawania za pomocą 
    odpowiedniego zdefiniowania przykładowych wektorów i ich sumy w funkcji main(). */
    if (sum1 == sum2)
    {
        cout << "Dodawanie wektorów jest przemienne." << endl;
    }
    else
    {
        cout << "Dodawanie wektorów NIE jest przemienne." << endl;
    }

    // 3. Znajdź kąt pomiędzy wektorem [0,3,0] a [5,5,0] 
    Vector3 v1(0, 3, 0);
    Vector3 v2(5, 5, 0);
    cout << "Kąt między wektorami: " << v1.angle(v2) << " stopni" << endl;

    // 4. Znajdź wektor prostopadły do wektorów [4,5,1] i [4,1,3]
    Vector3 u(4, 5, 1);
    Vector3 v(4, 1, 3);
    Vector3 perp = u.cross(v);
    cout << "Wektor prostopadly do [4,5,1] i [4,1,3]: " << perp << endl;
    
    // 5. Znormalizuj powstały wektor
    Vector3 perp_normalized = perp.normalize();
    cout << "Znormalizowany wektor prostopadly: " << perp_normalized << endl;

    // 6. Należy zaimplementować klasę wektor, promień, sfera i płaszczyzna
    // 7. Zdefiniować sferę S o środku w punkcie (0,0,0) i promieniu 10
    // 8. Zdefiniować promień R1 o początku w punkcie (0,0,-20) i skierowany w środek kuli. 
    // 9. Zdefiniować promień R2 o początku w tym samym punkcie, co R1, skierowany równolegle do osi Y. 

    Vector3 sphereCenter(0,0,0);
    Vector3 rayOrigin(0,0,-20);
    Sphere S(sphereCenter, 10.0);
    Ray R1(rayOrigin, (sphereCenter - rayOrigin).normalize());
    Ray R2(rayOrigin, Vector3(0,1,0));

    // 10. Proszę sprawdzić, czy istnieje przecięcie sfery S z promieniami R1 oraz R2.
    // 11. Wynik w postaci współrzędnych punktu przecięcia należy wyświetlić. 
    auto hit1 = S.intersects(R1);
    if (hit1)
    {
        cout << "R1 trafił w sfere S: " << *hit1 << endl;
    }
    else
    {
        cout << "R1 NIE trafił w sfere S" << endl;
    }

    auto hit2 = S.intersects(R2);

    if (hit2)
    {
        cout << "R2 trafił w sfere S: " << *hit2 << endl;
    }
    else
    {
        cout << "R2 NIE trafił w sfere S" << endl;
    }
    /* 12. Proszę zdefiniować dowolny promień R3, tak aby przecinał 
    on sferę S w dokładnie jednym punkcie. Podać współrzędne punktu przecięcia.*/
    Ray R3(Vector3(10, 0, -10), Vector3(0, 0, 1));
    auto hit3 = S.intersects(R3);
    if (hit3)
    {
        cout << "R3 trafił w sfere S: " << *hit3 << endl;
    }
    else
    {
        cout << "R3 NIE trafił w sfere S" << endl;
    }

    /* 13. Proszę zdefiniować płaszczyznę P przechodzącą przez punkt (0,0,0), 
    której wektor normalny tworzy kąt 45 stopni z osiami Y i Z. */
    Plane P(Vector3(0, 0, 0), Vector3(0, 1, 1));

    // 14. Proszę znaleźć punkt przecięcia płaszczyzny P z promieniem R2.
    auto hit4 = P.intersects(R2);
    if (hit4)
    {
        cout << "R2 trafił w płaszczyznę P: " << *hit4 << endl;
    }
    else
    {
        cout << "R2 NIE trafił w płaszczyznę P" << endl;
    }

    /* 15. Mamy trójkąt zdefiniowany przez punkty A(0, 0, 0), B(1,0, 0) i C(0, 1, 0). 
    Napisz kod, który sprawdzi, czy linia definiowana przez punkty P1 i P2 przecina ten trójkąt.
    Przetestuj kod dla poniższych przypadków: */
    Vector3 A(0, 0, 0);
    Vector3 B(1, 0, 0);
    Vector3 C(0, 1, 0);

    // Przypadek 1: Linia przechodząca przez trójkąt. 
    Vector3 p1_1(-1, 0.5, 0);
    Vector3 p2_1(1, 0.5, 0);
    cout << "Przypadek 1: " << (isLineIntersectingTriangle(p1_1, p2_1, A, B, C) ? "True" : "False") << endl;

    // Przypadek 2: Linia leżąca na płaszczyźnie trójkąta, ale poza nim. 
    Vector3 p1_2(2, -1, 0);
    Vector3 p2_2(2, 2, 0);
    cout << "Przypadek 2: " << (isLineIntersectingTriangle(p1_2, p2_2, A, B, C) ? "True" : "False") << endl;

    // Przypadek 3: Linia nieprzecinająca trójkąta. 
    Vector3 p1_3(0, 0, -1);
    Vector3 p2_3(0, 0, 1);
    cout << "Przypadek 3: " << (isLineIntersectingTriangle(p1_3, p2_3, A, B, C) ? "True" : "False") << endl;

    // -------------------------------
    // LAB 2
    // -------------------------------

    int width = 800;
    int height = 600;
    Image img(width, height);

    Sphere sphere(Vector3(0, 0, -5), 1.5);
    Plane plane(Vector3(0, -1.5, 0), Vector3(0, 1, 0));

    PerspectiveCamera camera(
        Vector3(0, 0, 0),
        Vector3(0, 0, -1),
        Vector3(0, 1, 0),
        90.0
    );

    cout << "Rozpoczynam renderowanie perspektywiczne z Antialiasingiem Adaptacyjnym..." << endl;

    int maxDepth = 2;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            
            Color pixelColor = adaptiveSample(x + 0.5, y + 0.5, 1.0, 0, maxDepth, camera, width, height, sphere, plane);

            img.setPixel(x, y, pixelColor);
        }
    }

    img.savePPM("perspective_adaptive_aa.ppm");
    cout << "Renderowanie zakonczone. Zapisano perspective_adaptive_aa.ppm" << endl;

    // orthographic_1spp.ppm
    return 0;
}
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "../include/Vector3.h"
#include "../include/Geometry.h"
#include "../include/Image.h"
#include "../include/Camera.h"

using namespace std;

double colorDifference(const Color& c1, const Color& c2) {
    return std::max({std::abs(c1.r - c2.r), std::abs(c1.g - c2.g), std::abs(c1.b - c2.b)});
}

Color trace(const Ray& ray, const Sphere& s1, const Sphere& s2) {
    auto hit1 = s1.intersects(ray);
    auto hit2 = s2.intersects(ray);

    double minDistance = 1e9; 
    Color finalColor(0, 0, 0);
    bool hitAnything = false;
    
    Vector3 hitPoint;
    Vector3 normal;
    Color baseColor;

    if (hit1) {
        double dist = (*hit1 - ray.origin).length();
        if (dist < minDistance) {
            minDistance = dist;
            hitPoint = *hit1;
            normal = (*hit1 - s1.center).normalize();
            baseColor = Color(0.1, 0.3, 0.9);
            hitAnything = true;
        }
    }

    if (hit2) {
        double dist = (*hit2 - ray.origin).length();
        if (dist < minDistance) {
            minDistance = dist;
            hitPoint = *hit2;
            normal = (*hit2 - s2.center).normalize();
            baseColor = Color(0.9, 0.2, 0.2);
            hitAnything = true;
        }
    }

    if (hitAnything) {
        Vector3 lightDir = Vector3(1, 1.5, 1).normalize();
        
        Color ambient = baseColor * 0.2;
        
        double diffuse = std::max(0.0, normal * lightDir);
        
        Vector3 viewDir = ray.direction * -1.0; 
        Vector3 reflectDir = (normal * (2.0 * (normal * lightDir)) - lightDir).normalize();
        double specular = std::pow(std::max(0.0, viewDir * reflectDir), 50.0);
        
        Ray shadowRay(hitPoint + lightDir * 0.001, lightDir);
        bool inShadow = false;
        
        if (s1.intersects(shadowRay) || s2.intersects(shadowRay)) {
            inShadow = true;
        }

        if (!inShadow) {
            finalColor = ambient + (baseColor * diffuse) + (Color(1, 1, 1) * specular);
        } else {
            finalColor = ambient; 
        }
    }
    else {
        double x = ray.origin.getX();
        double y = ray.origin.getY();

        int col = std::floor(x + 3.0);
        int row = std::floor(3.0 - y); 

        col = std::max(0, std::min(5, col));
        row = std::max(0, std::min(5, row));

        double t = row / 5.0; 

        if (col == 5) finalColor = Color(t, 0, 0);         
        else if (col == 4) finalColor = Color(0, t, 0);    
        else if (col == 3) finalColor = Color(0, 0, t);    
        else if (col == 2) finalColor = Color(1.0, 0, t);  
        else if (col == 1) finalColor = Color(0, 1.0, t);  
        else if (col == 0) finalColor = Color(1.0, 1.0, t);
    }

    finalColor.r = std::max(0.0, std::min(1.0, finalColor.r));
    finalColor.g = std::max(0.0, std::min(1.0, finalColor.g));
    finalColor.b = std::max(0.0, std::min(1.0, finalColor.b));

    return finalColor;
}

Color adaptiveSample(double x, double y, double size, int depth, int maxDepth, const Camera& camera, int width, int height, const Sphere& s1, const Sphere& s2) {
    Ray centerRay = camera.generateRay(x, y, width, height);
    Color centerColor = trace(centerRay, s1, s2);

    if (depth >= maxDepth) return centerColor;

    double quarterSize = size / 4.0;
    Ray r1 = camera.generateRay(x - quarterSize, y - quarterSize, width, height);
    Ray r2 = camera.generateRay(x + quarterSize, y - quarterSize, width, height);
    Ray r3 = camera.generateRay(x - quarterSize, y + quarterSize, width, height);
    Ray r4 = camera.generateRay(x + quarterSize, y + quarterSize, width, height);

    Color c1 = trace(r1, s1, s2);
    Color c2 = trace(r2, s1, s2);
    Color c3 = trace(r3, s1, s2);
    Color c4 = trace(r4, s1, s2);

    double threshold = 0.05; 
    bool needsSubdivision = 
        colorDifference(centerColor, c1) > threshold ||
        colorDifference(centerColor, c2) > threshold ||
        colorDifference(centerColor, c3) > threshold ||
        colorDifference(centerColor, c4) > threshold;

    if (needsSubdivision) {
        double halfSize = size / 2.0;
        c1 = adaptiveSample(x - quarterSize, y - quarterSize, halfSize, depth + 1, maxDepth, camera, width, height, s1, s2);
        c2 = adaptiveSample(x + quarterSize, y - quarterSize, halfSize, depth + 1, maxDepth, camera, width, height, s1, s2);
        c3 = adaptiveSample(x - quarterSize, y + quarterSize, halfSize, depth + 1, maxDepth, camera, width, height, s1, s2);
        c4 = adaptiveSample(x + quarterSize, y + quarterSize, halfSize, depth + 1, maxDepth, camera, width, height, s1, s2);
    }

    return (c1 + c2 + c3 + c4) * 0.25;
}

void renderScene(const Camera& camera, int width, int height, const std::string& filename, int maxDepth, const Sphere& s1, const Sphere& s2) {
    Image img(width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color pixelColor;
            
            if (maxDepth == 0) {
                Ray ray = camera.generateRay(x + 0.5, y + 0.5, width, height);
                pixelColor = trace(ray, s1, s2);
            } else {
                pixelColor = adaptiveSample(x + 0.5, y + 0.5, 1.0, 0, maxDepth, camera, width, height, s1, s2);
            }
            
            img.setPixel(x, y, pixelColor);
        }
    }
    img.savePPM(filename);
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
    int width = 600;
    int height = 600;

    Sphere sphereBlue(Vector3(0.0, 0.0, -2.0), 1.5);   
    Sphere sphereRed(Vector3(-1.5, 0.0, -3.0), 1.0);    

    OrthographicCamera orthoCam(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0), 6.0);
    PerspectiveCamera perspCam(Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0), 90.0);

    renderScene(orthoCam, width, height, "orthographic_1spp.ppm", 0, sphereBlue, sphereRed);
    renderScene(perspCam, width, height, "perspective_1spp.ppm", 0, sphereBlue, sphereRed);
    renderScene(perspCam, width, height, "perspective_4x4_aa.ppm", 2, sphereBlue, sphereRed);

    return 0;
}
#include <iostream>
#include "../include/Vector3.h"
#include "../include/Geometry.h"
#include "../include/Image.h"

using namespace std;

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

    int width = 256;
    int height = 256;
    Image img(width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Generujemy kolory w zależności od współrzędnych
            double r = double(x) / (width - 1);
            double g = double(height - 1 - y) / (height - 1);
            double b = 0.25;

            img.setPixel(x, y, Color(r, g, b));
        }
    }

    img.savePPM("test_render.ppm");
    return 0;
}
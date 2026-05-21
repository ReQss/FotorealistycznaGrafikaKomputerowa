#include <iostream>
#include <cmath>
#include <optional>
using namespace std;

// 1. Zaimplementuj klasę wektor wraz ze wszystkimi działaniami. 
class Vector3
{
private:
    double x;
    double y;
    double z;

public:

    Vector3()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    Vector3(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    double getX() const
    {
        return x;
    }

    double getY() const
    {
        return y;
    }

    double getZ() const
    {
        return z;
    }

    void setX(double x)
    {
        this->x = x;
    }

    void setY(double y)
    {
        this->y = y;
    }

    void setZ(double z)
    {
        this->z = z;
    }

    Vector3 operator+(const Vector3& other) const
    {
        return Vector3(
            x + other.x,
            y + other.y,
            z + other.z
        );
    }

    Vector3 operator-(const Vector3& other) const
    {
        return Vector3(
            x - other.x,
            y - other.y,
            z - other.z
        );
    }

    Vector3 operator*(double scalar) const
    {
        return Vector3(
            x * scalar,
            y * scalar,
            z * scalar
        );
    }

    Vector3 operator/(double scalar) const
    {
        return Vector3(
            x / scalar,
            y / scalar,
            z / scalar
        );
    }

    double operator*(const Vector3& other) const
    {
        return x * other.x +
               y * other.y +
               z * other.z;
    }

    bool operator==(const Vector3& other) const
    {
        return x == other.x &&
               y == other.y &&
               z == other.z;
    }

    double length() const
    {
        return sqrt(
            x * x +
            y * y +
            z * z
        );
    }

    double angle(const Vector3& other) const
    {
        double dot = (*this) * other;

        double len1 = length();
        double len2 = other.length();

        double cosValue = dot / (len1 * len2);

        return acos(cosValue) * 180.0 / M_PI;
    }

    Vector3 cross(const Vector3& other) const
    {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    Vector3 normalize() const
    {
        double len = length();
        if (len == 0) return Vector3(0, 0, 0);
        return (*this) / len;
    }

    friend ostream& operator<<(ostream& os, const Vector3& v)
    {
        os << "["
           << v.x << ", "
           << v.y << ", "
           << v.z << "]";

        return os;
    }
};

class Ray
{
public:
    Vector3 origin;
    Vector3 direction;

    Ray(const Vector3& origin, const Vector3& direction)
        : origin(origin), direction(direction.normalize()) {}
};

class Sphere
{
public:
    Vector3 center;
    double radius;

    Sphere(const Vector3& center, double radius)
        : center(center), radius(radius) {}
    optional<Vector3> intersects(const Ray& ray) const
    {
        Vector3 oc = ray.origin - center;

        double a = ray.direction * ray.direction;
        double b = 2.0 * (oc * ray.direction);
        double c = (oc * oc) - radius * radius;

        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0)
            return nullopt;

        double t = (-b - sqrt(discriminant)) / (2.0 * a);

        if (t < 0)
            return nullopt;

        return ray.origin + ray.direction * t;
    }
};

class Plane
{
public:
    Vector3 point;
    Vector3 normal;

    Plane(const Vector3& point, const Vector3& normal)
        : point(point), normal(normal.normalize()) {}

    optional<Vector3> intersects(const Ray& ray) const
    {
        double denom = normal * ray.direction; 

        if (abs(denom) > 1e-6) 
        {
            Vector3 p0l0 = point - ray.origin; 
            
            double t = (p0l0 * normal) / denom; 
            
            if (t >= 0) 
            {
                return ray.origin + ray.direction * t;
            }
        }
        return nullopt;
    }
};

bool isLineIntersectingTriangle(const Vector3& p1, const Vector3& p2, const Vector3& a, const Vector3& b, const Vector3& c) 
{
    const double EPSILON = 1e-6;
    
    Vector3 edge1 = b - a;
    Vector3 edge2 = c - a;
    
    Vector3 dir = p2 - p1;
    
    Vector3 h = dir.cross(edge2);
    double a_val = edge1 * h;

    if (a_val > -EPSILON && a_val < EPSILON)
        return false;

    double f = 1.0 / a_val;
    Vector3 s = p1 - a;
    double u = f * (s * h);

    if (u < 0.0 || u > 1.0)
        return false;

    Vector3 q = s.cross(edge1);
    double v = f * (dir * q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    double t = f * (edge2 * q);

    if (t > EPSILON && t <= 1.0 + EPSILON) 
        return true;

    return false;
}

int main()
{
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

    return 0;
}
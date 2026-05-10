#include <iostream>
#include <cmath>
#include <optional>
using namespace std;

class Vector3
{
private:
    double x;
    double y;
    double z;

public:

    // Default constructor
    Vector3()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    // Constructor with parameters
    Vector3(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    // Getters
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

    // Setters
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

    // Vector addition
    Vector3 operator+(const Vector3& other) const
    {
        return Vector3(
            x + other.x,
            y + other.y,
            z + other.z
        );
    }

    // Vector subtraction
    Vector3 operator-(const Vector3& other) const
    {
        return Vector3(
            x - other.x,
            y - other.y,
            z - other.z
        );
    }

    // Multiplication by scalar
    Vector3 operator*(double scalar) const
    {
        return Vector3(
            x * scalar,
            y * scalar,
            z * scalar
        );
    }

    // Division by scalar
    Vector3 operator/(double scalar) const
    {
        return Vector3(
            x / scalar,
            y / scalar,
            z / scalar
        );
    }

    // Dot product
    double operator*(const Vector3& other) const
    {
        return x * other.x +
               y * other.y +
               z * other.z;
    }

    // Equality comparison
    bool operator==(const Vector3& other) const
    {
        return x == other.x &&
               y == other.y &&
               z == other.z;
    }

    // Vector length
    double length() const
    {
        return sqrt(
            x * x +
            y * y +
            z * z
        );
    }

    // Angle between vectors
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
    // Display vector
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
    Vector3 point;      // Punkt na płaszczyźnie
    Vector3 normal;     // Wektor normalny

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

bool isLineIntersectingTriangle(const Vector3& p1, const Vector3& p2, 
                                const Vector3& a, const Vector3& b, const Vector3& c) 
{
    Vector3 u = b - a;
    Vector3 v = c - a;
    Vector3 n = u.cross(v);

    Vector3 dir = p2 - p1;
    double dot = n * dir;

    const double EPSILON = 1e-6;

    if (abs(dot) < EPSILON) 
    {
        double dist = n * (p1 - a);
        if (abs(dist) > EPSILON) 
        {
            return false;
        }

        Vector3 m = dir.cross(n);

        double d1 = m * (a - p1);
        double d2 = m * (b - p1);
        double d3 = m * (c - p1);

        bool hasPositive = (d1 > EPSILON) || (d2 > EPSILON) || (d3 > EPSILON);
        bool hasNegative = (d1 < -EPSILON) || (d2 < -EPSILON) || (d3 < -EPSILON);

        return hasPositive && hasNegative;
    } 
    else 
    {
        double t = (n * (a - p1)) / dot;
        Vector3 p = p1 + dir * t;

        Vector3 v0 = b - a;
        Vector3 v1 = c - a;
        Vector3 v2 = p - a;

        double d00 = v0 * v0;
        double d01 = v0 * v1;
        double d11 = v1 * v1;
        double d20 = v2 * v0;
        double d21 = v2 * v1;

        double denom = d00 * d11 - d01 * d01;
        double v_coord = (d11 * d20 - d01 * d21) / denom;
        double w_coord = (d00 * d21 - d01 * d20) / denom;
        double u_coord = 1.0 - v_coord - w_coord;

        return (u_coord > EPSILON) && (v_coord > EPSILON) && (w_coord > EPSILON);
    }
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

    /*prawdź czy prawidłowo działa
przemienność dodawania za pomocą odpowiedniego
zdefiniowania przykładowych wektorów i ich sumy w
funkcji main(). */
    if (sum1 == sum2)
    {
        cout << "Vector addition is commutative." << endl;
    }
    else
    {
        cout << "Vector addition is NOT commutative." << endl;
    }
    //3. Znajdź kąt pomiędzy wektorem [0,3,0] a [5,5,0] 
    // Angle between vectors
    Vector3 v1(0, 3, 0);
    Vector3 v2(5, 5, 0);

    cout << "Angle between vectors: "
         << v1.angle(v2)
         << " degrees" << endl;
    //4
    Vector3 u(4, 5, 1);
    Vector3 v(4, 1, 3);
    Vector3 perp = u.cross(v);
    cout << "Wektor prostopadly do [4,5,1] i [4,1,3]: " << perp << endl;
    //5
    Vector3 perp_normalized = perp / perp.length();
    cout << "Znormalizowany wektor prostopadly: " << perp_normalized << endl;

//6 Należy zaimplementować klasę wektor, promień, sfera i płaszczyzna
// 7 2. Zdefiniować sferę S o środku w punkcie (0,0,0) i promieniu 10
//8 . 3. Zdefiniować promień R1 o początku w punkcie (0,0,-20) i skierowany w środek kuli. 
//9Zdefiniować promień R2 o początku w tym samym punkcie, co R1, skierowany równolegle do osi Y. 

    Vector3 sphereCenter(0,0,0);
    Vector3 rayOrigin(0,0,-20);
    Sphere S(sphereCenter, 10.0);
    Ray R1(rayOrigin, (sphereCenter - rayOrigin).normalize());
    Ray R2(rayOrigin, Vector3(0,1,0));
    Plane plane(Vector3(0,0,0), Vector3(0,1,0));

// 10 Proszę sprawdzić, czy istnieje przecięcie sfery S z promieniami R1 oraz R2.
// 11. Wynik w postaci współrzędnych punktu przecięcia należy wyświetlić. 
    auto hit1 = S.intersects(R1);
    if (hit1)
    {
        cout << "R1 hit: " << *hit1 << endl;
    }
    else
    {
        cout << "R1 no hit" << endl;
    }

    auto hit2 = S.intersects(R2);

    if (hit2)
    {
        cout << "R2 hit: " << *hit2 << endl;
    }
    else
    {
        cout << "R2 no hit" << endl;
    }
    /* 12 Proszę zdefiniować dowolny promień R3, tak aby
przecinał on sferę S w dokładnie jednym punkcie. Podać
współrzędne punktu przecięcia.*/
    Ray R3(Vector3(10, 0, -10), Vector3(0, 0, 1));
    auto hit3 = S.intersects(R3);
    if (hit3)
    {
        cout << "R3 hit (punkt stycznosci): " << *hit3 << endl;
    }
    else
    {
        cout << "R3 no hit" << endl;
    }

    /* 13. Proszę zdefiniować płaszczyznę P przechodzącą przez punkt (0,0,0), 
    której wektor normalny tworzy kąt 45 stopni z osiami Y i Z. */
    Plane P(Vector3(0, 0, 0), Vector3(0, 1, 1));

    // 14. Proszę znaleźć punkt przecięcia płaszczyzny P z promieniem R2.
    auto hit4 = P.intersects(R2);
    if (hit4)
    {
        cout << "R2 hit Plane P: " << *hit4 << endl;
    }
    else
    {
        cout << "R2 no hit Plane P" << endl;
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
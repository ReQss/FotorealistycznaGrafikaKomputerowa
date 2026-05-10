#include <iostream>
#include <cmath>

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
    bool intersects(const Ray& ray) const
    {
        Vector3 oc = ray.origin - center;

        double a = ray.direction * ray.direction;
        double b = 2.0 * (oc * ray.direction);
        double c = (oc * oc) - radius * radius;

        double discriminant = b * b - 4 * a * c;

        return discriminant >= 0;
    }
};

class Plane
{
public:
    Vector3 point;      // Punkt na płaszczyźnie
    Vector3 normal;     // Wektor normalny

    Plane(const Vector3& point, const Vector3& normal)
        : point(point), normal(normal.normalize()) {}
};
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
    if (S.intersects(R1))
    cout << "R1 intersects sphere" << endl;
    else
        cout << "R1 does NOT intersect sphere" << endl;

    if (S.intersects(R2))
        cout << "R2 intersects sphere" << endl;
    else
        cout << "R2 does NOT intersect sphere" << endl;
    return 0;
}
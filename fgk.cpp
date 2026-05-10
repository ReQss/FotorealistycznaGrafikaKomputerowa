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

    // Check commutativity
    if (sum1 == sum2)
    {
        cout << "Vector addition is commutative." << endl;
    }
    else
    {
        cout << "Vector addition is NOT commutative." << endl;
    }

    // Angle between vectors
    Vector3 v1(0, 3, 0);
    Vector3 v2(5, 5, 0);

    cout << "Angle between vectors: "
         << v1.angle(v2)
         << " degrees" << endl;

    Vector3 u(4, 5, 1);
    Vector3 v(4, 1, 3);
    Vector3 perp = u.cross(v);
    cout << "Wektor prostopadly do [4,5,1] i [4,1,3]: " << perp << endl;

    Vector3 perp_normalized = perp / perp.length();
    cout << "Znormalizowany wektor prostopadly: " << perp_normalized << endl;
    return 0;
}
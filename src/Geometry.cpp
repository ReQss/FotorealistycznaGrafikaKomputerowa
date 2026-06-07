#include "../include/Geometry.h"
#include <cmath>

using namespace std;

// ---------- Ray ----------
Ray::Ray(const Vector3& origin, const Vector3& direction)
    : origin(origin), direction(direction.normalize()) {}

// ---------- Sphere ----------
Sphere::Sphere(const Vector3& center, double radius, const Material& material)
    : center(center), radius(radius), material(material) {}

optional<HitRecord> Sphere::intersects(const Ray& ray) const
{
    Vector3 oc = ray.origin - center;
    double a = ray.direction * ray.direction;
    double b = 2.0 * (oc * ray.direction);
    double c = (oc * oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return nullopt;

    double t = (-b - sqrt(discriminant)) / (2.0 * a);
    if (t < 1e-4) 
    {
        t = (-b + sqrt(discriminant)) / (2.0 * a);
        if (t < 1e-4) return nullopt;
    }

    HitRecord rec;
    rec.t      = t;
    rec.point  = ray.origin + ray.direction * t;
    rec.normal = (rec.point - center).normalize();
    rec.material = material;
    return rec;
}

// ---------- Plane ----------
Plane::Plane(const Vector3& point, const Vector3& normal, const Material& material)
    : point(point), normal(normal.normalize()), material(material) {}

optional<HitRecord> Plane::intersects(const Ray& ray) const
{
    double denom = normal * ray.direction;
    if (fabs(denom) > 1e-6)
    {
        Vector3 p0l0 = point - ray.origin;
        double t = (p0l0 * normal) / denom;
        if (t >= 1e-4)
        {
            HitRecord rec;
            rec.t        = t;
            rec.point    = ray.origin + ray.direction * t;
            rec.normal   = normal;
            rec.material = material;
            return rec;
        }
    }
    return nullopt;
}

// ---------- Triangle ----------
bool isLineIntersectingTriangle(const Vector3& p1, const Vector3& p2,
                                const Vector3& a,  const Vector3& b, const Vector3& c)
{
    const double EPSILON = 1e-6;
    Vector3 edge1 = b - a;
    Vector3 edge2 = c - a;
    Vector3 dir   = p2 - p1;
    Vector3 h     = dir.cross(edge2);
    double  a_val = edge1 * h;

    if (a_val > -EPSILON && a_val < EPSILON) return false;

    double  f = 1.0 / a_val;
    Vector3 s = p1 - a;
    double  u = f * (s * h);
    if (u < 0.0 || u > 1.0) return false;

    Vector3 q = s.cross(edge1);
    double  v = f * (dir * q);
    if (v < 0.0 || u + v > 1.0) return false;

    double t = f * (edge2 * q);
    return (t > EPSILON && t <= 1.0 + EPSILON);
}
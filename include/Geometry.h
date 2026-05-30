#pragma once
#include "Vector3.h"
#include "Material.h"
#include <optional>

class Ray
{
public:
    Vector3 origin;
    Vector3 direction;

    Ray(const Vector3& origin, const Vector3& direction);
};

struct HitRecord
{
    Vector3 point;
    Vector3 normal;
    double  t;
    Material material;
};

class Sphere
{
public:
    Vector3  center;
    double   radius;
    Material material;

    Sphere(const Vector3& center, double radius, const Material& material = Material());
    std::optional<HitRecord> intersects(const Ray& ray) const;
};

class Plane
{
public:
    Vector3  point;
    Vector3  normal;
    Material material;

    Plane(const Vector3& point, const Vector3& normal, const Material& material = Material());
    std::optional<HitRecord> intersects(const Ray& ray) const;
};

bool isLineIntersectingTriangle(const Vector3& p1, const Vector3& p2,
                                const Vector3& a,  const Vector3& b, const Vector3& c);
#pragma once
#include "../include/Vector3.h"
#include <optional>

class Ray
{
public:
    Vector3 origin;
    Vector3 direction;

    Ray(const Vector3& origin, const Vector3& direction);
};

class Sphere
{
public:
    Vector3 center;
    double radius;

    Sphere(const Vector3& center, double radius);
    std::optional<Vector3> intersects(const Ray& ray) const;
};

class Plane
{
public:
    Vector3 point;
    Vector3 normal;

    Plane(const Vector3& point, const Vector3& normal);
    std::optional<Vector3> intersects(const Ray& ray) const;
};

bool isLineIntersectingTriangle(const Vector3& p1, const Vector3& p2, const Vector3& a, const Vector3& b, const Vector3& c);
#pragma once
#include "Geometry.h"
#include <cmath>

class Camera 
{
public:
    virtual ~Camera() = default;
    
    virtual Ray generateRay(double x, double y, double width, double height) const = 0;
};

class OrthographicCamera : public Camera 
{
private:
    Vector3 origin;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    double scale;

public:
    OrthographicCamera(const Vector3& origin, const Vector3& direction, const Vector3& upVector, double scale)
        : origin(origin), scale(scale) 
    {
        
        forward = direction.normalize();
        right = upVector.cross(forward).normalize();
        up = forward.cross(right).normalize();
    }

    Ray generateRay(double x, double y, double width, double height) const override 
    {
        double aspectRatio = width / height;

        double ndcX = (2.0 * x / (width - 1)) - 1.0;
        double ndcY = 1.0 - (2.0 * y / (height - 1));

        Vector3 rayOrigin = origin + right * (ndcX * scale * aspectRatio) + up * (ndcY * scale);

        return Ray(rayOrigin, forward);
    }
};

class PerspectiveCamera : public Camera 
{
private:
    Vector3 origin;
    Vector3 forward;
    Vector3 right;
    Vector3 up;
    double fov;

public:
    PerspectiveCamera(const Vector3& origin, const Vector3& direction, const Vector3& upVector, double fov)
        : origin(origin), fov(fov) 
    {
        forward = direction.normalize();
        right = upVector.cross(forward).normalize();
        up = forward.cross(right).normalize();
    }

    Ray generateRay(double x, double y, double width, double height) const override 
    {
        double aspectRatio = width / height;
        
        double fovRadians = fov * M_PI / 180.0;
        double scaleFactor = tan(fovRadians / 2.0);

        double ndcX = (2.0 * x / (width - 1)) - 1.0;
        double ndcY = 1.0 - (2.0 * y / (height - 1));

        Vector3 rayDirection = forward + right * (ndcX * scaleFactor * aspectRatio) + up * (ndcY * scaleFactor);

        return Ray(origin, rayDirection.normalize());
    }
};
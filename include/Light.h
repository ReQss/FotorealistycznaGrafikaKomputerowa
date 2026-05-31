#pragma once
#include "Vector3.h"
#include "Color.h"
#include "Material.h"
#include "Geometry.h"
#include <vector>


class Light
{
public:
    Color intensity; 

    Light(const Color& intensity) : intensity(intensity) {}
    virtual ~Light() = default;

    virtual Vector3 directionFrom(const Vector3& point) const = 0;

    virtual double distanceFrom(const Vector3& point) const = 0;

    virtual Color getDiffuse(const Vector3& cameraPosition,
                             const HitRecord& iInfo) const = 0;

    virtual Color getSpecular(const Vector3& cameraPosition,
                              const HitRecord& iInfo) const = 0;

    virtual int isInShadow(const HitRecord& iInfo,
                           const std::vector<Sphere>& spheres,
                           const std::vector<Plane>&  planes) const = 0;
};

class PointLight : public Light
{
public:
    Vector3 position;    
    double constAtten;
    double linearAtten;
    double quadAtten;

    PointLight(const Vector3& position, const Color& intensity,
               double constAtten  = 1.0,
               double linearAtten = 0.0,
               double quadAtten   = 0.0)
        : Light(intensity),
          position(position),
          constAtten(constAtten),
          linearAtten(linearAtten),
          quadAtten(quadAtten)
    {}

    Vector3 directionFrom(const Vector3& point) const override
    {
        return (position - point).normalize();
    }

    double distanceFrom(const Vector3& point) const override
    {
        return (position - point).length();
    }

    double attenuationAt(double distance) const
    {
        return 1.0 / (constAtten +
                      linearAtten * distance +
                      quadAtten   * distance * distance);
    }

    Color getDiffuse(const Vector3& cameraPosition,
                     const HitRecord& iInfo) const override
    {
        Vector3 L    = directionFrom(iInfo.point);
        double  dist = distanceFrom(iInfo.point);
        double  atten = attenuationAt(dist);

        double diff = std::max(0.0, iInfo.normal * L);

        return iInfo.material.Kd * intensity * (diff * atten);
    }

    Color getSpecular(const Vector3& cameraPosition,
                      const HitRecord& iInfo) const override
    {
        Vector3 L    = directionFrom(iInfo.point);
        double  dist = distanceFrom(iInfo.point);
        double  atten = attenuationAt(dist);

        Vector3 V = (cameraPosition - iInfo.point).normalize();

        Vector3 R = (iInfo.normal * (2.0 * (iInfo.normal * L)) - L).normalize();

        double spec = std::pow(std::max(0.0, V * R), iInfo.material.shininess);

        return iInfo.material.Ks * intensity * (spec * atten);
    }

    int isInShadow(const HitRecord& iInfo,
                   const std::vector<Sphere>& spheres,
                   const std::vector<Plane>&  planes) const override
    {
        Vector3 L    = directionFrom(iInfo.point);
        double  dist = distanceFrom(iInfo.point);

        Ray shadowRay(iInfo.point + L * 1e-4, L);

        for (const auto& sphere : spheres)
        {
            auto hit = sphere.intersects(shadowRay);
            if (hit && hit->t < dist) return 1;
        }
        for (const auto& plane : planes)
        {
            auto hit = plane.intersects(shadowRay);
            if (hit && hit->t < dist) return 1;
        }
        return 0;
    }
};
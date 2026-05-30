#pragma once
#include "Vector3.h"
#include "Color.h"

// Abstrakcyjna klasa bazowa zrodla swiatla.
class Light
{
public:
    Color intensity; // natezenie / kolor swiatla

    Light(const Color& intensity) : intensity(intensity) {}
    virtual ~Light() = default;

    // Zwraca znormalizowany kierunek od punktu 'point' do zrodla swiatla.
    virtual Vector3 directionFrom(const Vector3& point) const = 0;

    // Zwraca odleglosc od punktu 'point' do zrodla (inf dla kierunkowych).
    virtual double distanceFrom(const Vector3& point) const = 0;
};

// Punktowe zrodlo swiatla - emituje we wszystkich kierunkach z jednego miejsca.
class PointLight : public Light
{
public:
    Vector3 position;

    // constAtten, linearAtten, quadAtten - wspolczynniki zanikania
    double constAtten;
    double linearAtten;
    double quadAtten;

    PointLight(const Vector3& position, const Color& intensity,
               double constAtten = 1.0,
               double linearAtten = 0.0,
               double quadAtten  = 0.0)
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

    // Wspolczynnik zanikania natezenia w zaleznosci od odleglosci
    double attenuationAt(double distance) const
    {
        return 1.0 / (constAtten +
                      linearAtten * distance +
                      quadAtten   * distance * distance);
    }
};
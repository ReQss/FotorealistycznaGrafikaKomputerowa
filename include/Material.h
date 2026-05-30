#pragma once
#include "Color.h"

// Klasa odpowiedzialna za material obiektu w scenie.
// Przechowuje wspolczynniki modelu oswietlenia Phonga:
//   Ka - ambient (oswietlenie otoczenia)
//   Kd - diffuse (rozproszenie lambertowskie)
//   Ks - specular (blask lustrzany)
//   shininess - wykładnik Phonga (ostrosc rozblysku)
class Material
{
public:
    Color  Ka;        // ambient color
    Color  Kd;        // diffuse color
    Color  Ks;        // specular color
    double shininess; // wykladnik Phonga (n)

    Material()
        : Ka(0.1, 0.1, 0.1),
          Kd(0.8, 0.8, 0.8),
          Ks(1.0, 1.0, 1.0),
          shininess(32.0)
    {}

    Material(const Color& ambient, const Color& diffuse,
             const Color& specular, double shininess)
        : Ka(ambient), Kd(diffuse), Ks(specular), shininess(shininess)
    {}
};
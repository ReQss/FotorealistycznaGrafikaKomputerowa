#pragma once
#include "Color.h"

class Material
{
public:
    Color  Ka;       
    Color  Kd;       
    Color  Ks;       
    double shininess; 

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
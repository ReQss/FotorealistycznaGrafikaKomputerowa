#include "../include/Image.h"
#include <fstream>
#include <iostream>
#include <algorithm>

Image::Image(int w, int h) : width(w), height(h) 
{
    pixels.resize(width * height);
}

void Image::setPixel(int x, int y, const Color& color) 
{
    if (x >= 0 && x < width && y >= 0 && y < height) 
    {
        pixels[y * width + x] = color;
    }
}

void Image::savePPM(const std::string& filename) const 
{
    std::ofstream out(filename);
    if (!out) 
    {
        std::cerr << "Nie mozna utworzyc pliku: " << filename << std::endl;
        return;
    }

    out << "P3\n" << width << " " << height << "\n255\n";

    for (const auto& pixel : pixels) {
        int ir = static_cast<int>(255.999 * std::clamp(pixel.r, 0.0, 1.0));
        int ig = static_cast<int>(255.999 * std::clamp(pixel.g, 0.0, 1.0));
        int ib = static_cast<int>(255.999 * std::clamp(pixel.b, 0.0, 1.0));

        out << ir << " " << ig << " " << ib << "\n";
    }
    
    out.close();
    std::cout << "Zapisano obraz do pliku: " << filename << std::endl;
}
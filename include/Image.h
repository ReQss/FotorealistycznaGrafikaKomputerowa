#pragma once
#include "Color.h"
#include <vector>
#include <string>

class Image 
{
private:
    int width;
    int height;
    std::vector<Color> pixels;

public:
    Image(int w, int h);
    
    void setPixel(int x, int y, const Color& color);
    void savePPM(const std::string& filename) const;
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};
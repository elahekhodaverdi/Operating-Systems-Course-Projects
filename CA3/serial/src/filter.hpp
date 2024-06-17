#ifndef __FILTER_HPP
#define __FILTER_HPP

#include <array>
#include "bmp.hpp"
#include <iostream>
#include <cmath>

namespace filter {

typedef std::array<float, 9> Kernel;

void flip(BMPImage img);
void blur(BMPImage img);
void purplehaze(BMPImage img);
void sepia(BMPImage img);
void lines(BMPImage img);
void kernel(BMPImage img, Kernel kernel);
void setLine(BMPImage img,int width, int row1, int col1, int row2, int col2, unsigned char value);
}
#endif
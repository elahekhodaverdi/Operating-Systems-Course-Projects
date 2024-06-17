#ifndef __FILTER_HPP
#define __FILTER_HPP

#include <array>
#include "bmp.hpp"
#include "thread.hpp"
#include <cmath>

namespace filter {

typedef std::array<float, 9> Kernel;

void *flip(void* thread_arg);
void *blur(void* thread_arg);
void *purplehaze(void* thread_arg);
void *sepia(void* thread_arg);
void *lines(void* thread_arg);
void kernel(long tid, BMPImage& img, BMPImage& copy, Kernel kernel);
void setLine(BMPImage& img,int width, int row1, int col1, int row2, int col2, unsigned char value);

}
#endif
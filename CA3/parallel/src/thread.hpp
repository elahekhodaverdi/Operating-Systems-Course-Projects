#ifndef __THREAD_HPP
#define __THREAD_HPP

#include <pthread.h>
#include <iostream>
#include "bmp.hpp"

struct ThreadArgs
{
    ThreadArgs(long _tid, BMPImage &_img, BMPImage &_copy_img) : tid(_tid), img(_img), copy_img(_copy_img) {}
    long tid;
    BMPImage& img;
    BMPImage& copy_img;
};

void *read_parallel(void *args);
void *write_parallel(void *args);
void create_threads(void *(*func)(void *), BMPImage &img, BMPImage &copy_img);
#endif

#ifndef __BMP_HPP
#define __BMP_HPP

#define OUTPUT_FILE "smaple.bmp"
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <cstring>

#define NUMBER_OF_THREADS 6
typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)

struct RGB
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

class BMPImage
{
private:
    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;
    RGB **pixels;
    int padding;
    char *filebuffer;

public:
    BMPImage(const char *filepath);
    BMPImage(const BMPImage&other);
    BMPImage() = default;
    bool fill_and_allocate(char *&buffer, const char *filepath);
    void write(long tid);
    void read(long tid);
    RGB &operator()(int row, int col);
    void flip(long tid);
    bool inImage(int row, int col);
    void dealloce_memory();
    int get_height() const { return info_header->biHeight; }
    int get_width() const { return info_header->biWidth; }
    void write_to_file();
};

#endif
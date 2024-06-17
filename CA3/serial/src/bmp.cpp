#include "bmp.hpp"



BMPImage::BMPImage(const char* filepath){
    if (!fill_and_allocate(filebuffer, filepath)) {
        std::cout << "File read error" << std::endl;
        exit(1);
    }
    pixels = new RGB*[info_header->biHeight];
    for(int i=0; i< info_header->biHeight; i++)
        pixels[i] = new RGB[info_header->biWidth];
}


bool BMPImage::fill_and_allocate(char*& buffer, const char* filepath){
    std::ifstream file(filepath);
    if (!file) {
        std::cout << "File " << filepath << " doesn't exist!" << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    return true;
}


void BMPImage::read() {
    int count = 1;
    int extra = info_header->biWidth % 4;
    for (int i = 0; i < info_header->biHeight; i++) {
        count += extra;
        for (int j = info_header->biWidth - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    pixels[i][j].red = filebuffer[file_header->bfSize - count];
                    break;
                case 1:
                    pixels[i][j].green = filebuffer[file_header->bfSize - count];
                    break;
                case 2:
                    pixels[i][j].blue = filebuffer[file_header->bfSize - count];
                    break;
                }
                count++;
            }
        }
    }
}


void BMPImage::write() {
    std::ofstream write(OUTPUT_FILE);
    if (!write) {
        std::cout << "Failed to write " << OUTPUT_FILE << std::endl;
        return;
    }
    int count = 1;
    int extra = info_header->biWidth % 4;
    for (int i = 0; i < info_header->biHeight; i++) {
        count += extra;
        for (int j = info_header->biWidth - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    filebuffer[file_header->bfSize - count] = pixels[i][j].red;
                    break;
                case 1:
                    filebuffer[file_header->bfSize - count] = pixels[i][j].green;
                    break;
                case 2:
                    filebuffer[file_header->bfSize - count] = pixels[i][j].blue;
                    break;
                }
                count++;
            }
        }
    }
    write.write(filebuffer, file_header->bfSize);
}


bool BMPImage::inImage(int row, int col){
    return ( (row >= 0) && (row < info_header->biHeight) && (col >= 0) && (col < info_header->biWidth));
}

void BMPImage::flip(){
    for (int i = 0; i < info_header->biHeight/2; ++i) {
        for (int j = 0; j < info_header->biWidth; ++j) {
            std::swap(pixels[i][j], pixels[info_header->biHeight - i - 1][j]);
        }
    }
}

RGB& BMPImage::operator()(int row, int col){
    return pixels[row][col];
}
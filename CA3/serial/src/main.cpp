#include "bmp.hpp"
#include "filter.hpp"
#include <chrono>
#include <unistd.h>
#include <fstream>
int main(int argc, char *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();
    BMPImage img(argv[1]);

    auto start_read = std::chrono::high_resolution_clock::now();
    img.read();
    auto end_read = std::chrono::high_resolution_clock::now();

    filter::flip(img);
    auto end_flip = std::chrono::high_resolution_clock::now();

    filter::blur(img);
    auto end_blur = std::chrono::high_resolution_clock::now();

    filter::purplehaze(img);
    auto end_purple = std::chrono::high_resolution_clock::now();

    filter::lines(img);
    auto end_lines = std::chrono::high_resolution_clock::now();

    img.write();
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Read: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_read - start_read).count() << " ms" << std::endl;
    std::cout << "Flip: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_flip - end_read).count() << " ms" << std::endl;
    std::cout << "Blur: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_blur - end_flip).count() << " ms" << std::endl;
    std::cout << "Purple: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_purple - end_blur).count() << " ms" << std::endl;
    std::cout << "Lines: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_lines - end_purple).count() << " ms" << std::endl;
    std::cout << "Write: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - end_lines).count() << " ms" << std::endl;
    std::cout << "Execution: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

    return 0;
}
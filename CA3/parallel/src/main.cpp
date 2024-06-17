#include "bmp.hpp"
#include "filter.hpp"
#include <chrono>
#include <unistd.h>
#include <fstream>
#include "thread.hpp"

void image_processing(char *filepath)
{
    auto start = std::chrono::high_resolution_clock::now();
    BMPImage img(filepath);
    auto start_read = std::chrono::high_resolution_clock::now();

    create_threads(read_parallel,img,img);
    auto end_read = std::chrono::high_resolution_clock::now();

    BMPImage copy_img(img);

    create_threads(filter::flip,img,img);
    auto end_flip = std::chrono::high_resolution_clock::now();

    create_threads(filter::blur,img,copy_img);
    auto end_blur = std::chrono::high_resolution_clock::now();

    create_threads(filter::purplehaze,copy_img,copy_img);
    auto end_purple = std::chrono::high_resolution_clock::now();

    create_threads(filter::lines,copy_img,copy_img);
    auto end_lines = std::chrono::high_resolution_clock::now();

    create_threads(write_parallel,copy_img,copy_img);
    copy_img.write_to_file();
    auto end = std::chrono::high_resolution_clock::now();

    copy_img.dealloce_memory();
    img.dealloce_memory();


    std::cout << "Read: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_read - start_read).count() << " ms" << std::endl;
    std::cout << "Flip: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_flip - end_read).count() << " ms" << std::endl;
    std::cout << "Blur: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_blur - end_flip).count() << " ms" << std::endl;
    std::cout << "Purple: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_purple - end_blur).count() << " ms" << std::endl;
    std::cout << "Lines: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_lines - end_purple).count() << " ms" << std::endl;
    std::cout << "Write: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - end_lines).count() << " ms" << std::endl;
    std::cout << "Execution: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
}

int main(int argc, char *argv[])
{
    image_processing(argv[1]);
    return 0;
}
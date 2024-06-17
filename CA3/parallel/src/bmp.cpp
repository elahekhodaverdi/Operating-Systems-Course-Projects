#include "bmp.hpp"

BMPImage::BMPImage(const char *filepath)
{
    if (!fill_and_allocate(filebuffer, filepath))
    {
        std::cout << "File read error" << std::endl;
        exit(1);
    }
    pixels = new RGB *[info_header->biHeight];
    for (int i = 0; i < info_header->biHeight; i++)
        pixels[i] = new RGB[info_header->biWidth];
}

bool BMPImage::fill_and_allocate(char *&buffer, const char *filepath)
{
    std::ifstream file(filepath);
    if (!file)
    {
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

void BMPImage::read(long tid)
{
    int start_row = float(float(info_header->biHeight) / NUMBER_OF_THREADS) * tid;
    int end_row = float(float(info_header->biHeight) / NUMBER_OF_THREADS) * (tid + 1);
    int extra = info_header->biWidth % 4;
    int count = (tid * (info_header->biHeight / NUMBER_OF_THREADS)) * (info_header->biWidth * 3 + extra) + 1;
    for (int i = start_row; i < end_row; i++)
    {
        count += extra;
        for (int j = info_header->biWidth - 1; j >= 0; j--)
        {
            for (int k = 0; k < 3; k++)
            {
                switch (k)
                {
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
    pthread_exit(NULL);
}

void BMPImage::write_to_file()
{
    std::ofstream write(OUTPUT_FILE);
    if (!write)
    {
        std::cout << "Failed to write " << OUTPUT_FILE << std::endl;
        return;
    }
    write.write(filebuffer, file_header->bfSize);
}
void BMPImage::write(long tid)
{
    int start_row = float(float(info_header->biHeight) / NUMBER_OF_THREADS) * tid;
    int end_row = float(float(info_header->biHeight) / NUMBER_OF_THREADS) * (tid + 1);
    int extra = info_header->biWidth % 4;
    int count = (tid * (info_header->biHeight / NUMBER_OF_THREADS)) * (info_header->biWidth * 3 + extra) + 1;
    for (int i = start_row; i < end_row; i++)
    {
        count += extra;
        for (int j = info_header->biWidth - 1; j >= 0; j--)
        {
            for (int k = 0; k < 3; k++)
            {
                switch (k)
                {
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
    pthread_exit(NULL);
}

void BMPImage::dealloce_memory()
{
    for (int i = 0; i < info_header->biHeight; i++)
        delete pixels[i];
    delete pixels;
    delete filebuffer;
}

bool BMPImage::inImage(int row, int col)
{
    return ((row >= 0) && (row < info_header->biHeight) && (col >= 0) && (col < info_header->biWidth));
}

void BMPImage::flip(long tid)
{
    int start_col = float(float(info_header->biWidth) / NUMBER_OF_THREADS) * tid;
    int end_col = float(float(info_header->biWidth) / NUMBER_OF_THREADS) * (tid + 1);

    for (int i = 0; i < info_header->biHeight / 2; ++i)
    {
        for (int j = start_col; j < end_col; ++j)
        {
            std::swap(pixels[i][j], pixels[info_header->biHeight - i - 1][j]);
        }
    }
    pthread_exit(NULL);
}


RGB &BMPImage::operator()(int row, int col)
{
    return pixels[row][col];
}

BMPImage::BMPImage(const BMPImage &other)
{
    padding = other.padding;
    file_header = other.file_header;
    info_header = other.info_header;
    filebuffer = new char[other.file_header->bfSize];
    memcpy(filebuffer, other.filebuffer, other.file_header->bfSize);
    pixels = new RGB *[info_header->biHeight];
    for (int i = 0; i < info_header->biHeight; i++)
    {
        pixels[i] = new RGB[info_header->biWidth];
        for (int j = 0; j < info_header->biWidth; j++)
        {
            pixels[i][j] = other.pixels[i][j];
        }
    }
}
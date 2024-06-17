#include "filter.hpp"

namespace filter
{

void kernel(BMPImage img, Kernel kernel)
{
    static const int dir_x[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    static const int dir_y[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};

    for (int row = 0; row < img.get_height(); ++row)
    {
        for (int col = 0; col < img.get_width(); ++col)
        {
            float red = 0, green = 0, blue = 0;
            for (int i = 0; i < 9; ++i)
            {
                RGB pixel = img(row, col);
                if (img.inImage(row + dir_x[i], col + dir_y[i]))
                {
                    pixel = img(row + dir_x[i], col + dir_y[i]);
                }
                red += kernel[i] * pixel.red;
                green += kernel[i] * pixel.green;
                blue += kernel[i] * pixel.blue;
            }
            auto &pixel = img(row, col);
            pixel.red = std::min(255, std::max<int>(0, red));
            pixel.green = std::min(255, std::max<int>(0, green));
            pixel.blue = std::min(255, std::max<int>(0, blue));
        }
    }
}

void flip(BMPImage img)
{
    img.flip();
}
void blur(BMPImage img)
{
    static const Kernel blur_kernel = {1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0, 
                                        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
                                         1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0};
    kernel(img, blur_kernel);
}
void purplehaze(BMPImage img)
{
    int height = img.get_height();
    int width = img.get_width();
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            auto &pixel = img(i, j);
            float red = 0, green = 0, blue = 0;
            red = 0.5 * pixel.red + 0.3 * pixel.green + 0.5 * pixel.blue;
            green = 0.16 * pixel.red + 0.5 * pixel.green + 0.16 * pixel.blue;
            blue = 0.6 * pixel.red + 0.2 * pixel.green + 0.8 * pixel.blue;
            pixel.red = std::min(255, std::max<int>(0, red));
            pixel.green = std::min(255, std::max<int>(0, green));
            pixel.blue = std::min(255, std::max<int>(0, blue));
        }
    }
}

void lines(BMPImage img)
{
    int height = img.get_height();
    int width = img.get_width();
    setLine(img, width, 0, width/2, height/2, 0, 255);
    setLine(img, width, 0, width-1, height-1, 0, 255);
    setLine(img, width, height/2, width-1, height-1, width/2, 255);
}

void sepia(BMPImage img)
{
    int height = img.get_height();
    int width = img.get_width();
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            auto &pixel = img(i, j);
            float red = 0, green = 0, blue = 0;
            red = 0.393 * pixel.red + 0.769 * pixel.green + 0.189 * pixel.blue;
            green = 0.349 * pixel.red + 0.686 * pixel.green + 0.168 * pixel.blue;
            blue = 0.272 * pixel.red + 0.534 * pixel.green + 0.131 * pixel.blue;
            pixel.red = std::min(255, std::max<int>(0, red));
            pixel.green = std::min(255, std::max<int>(0, green));
            pixel.blue = std::min(255, std::max<int>(0, blue));
        }
    }
}

void setLine(BMPImage img, int width, int row1, int col1, int row2, int col2, unsigned char value)
{
    float slope = static_cast<float>(col2 - col1) / (row2 - row1);

    float y_intercept = col1 - slope * row1;
    for (int x = row1; x <= row2; ++x)
    {
        int y = static_cast<int>(round(slope * x + y_intercept));
        auto &pixel = img(x, y);
        if (y >= 0 && y < width)
        {
            pixel.blue = value;
            pixel.green = value;
            pixel.red = value;
        }
    }
}

}
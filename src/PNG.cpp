#include <iostream>
#include "lodepng/lodepng.h"
#include "PNG.h"

PNG::PNG()
{
    width = 0;
    height = 0;
}

PNG::PNG(const char * filename)
{
    lodepng::decode(rawdata, width, height, filename);
}

PNG::PNG(unsigned int width, unsigned int height)
{
    rawdata = vector<unsigned char>(width * height * 4, 255);
    this->width = width;
    this->height = height;
}

PNG::PNG(PNG & other)
{
    this->rawdata = other.rawdata;
    this->width = other.width;
    this->height = other.height;
}

unsigned PNG::getWidth()
{
    return width;
}

unsigned PNG::getHeight()
{
    return height;
}

Pixel PNG::getPixel(unsigned int x, unsigned int y)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        cout << __FUNCTION__ << ": ERROR invalid coordinates x=" << x << " y=" << y << endl;
        return Pixel();
    }
    unsigned char r = rawdata[(x + (y * width)) * 4];
    unsigned char g = rawdata[(x + (y * width)) * 4 + 1];
    unsigned char b = rawdata[(x + (y * width)) * 4 + 2];
    unsigned char a = rawdata[(x + (y * width)) * 4 + 3];
    Pixel p(r, g, b, a);
    return p;
}

bool PNG::setPixel(unsigned int x, unsigned int y, Pixel p)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        cout << __FUNCTION__ << ": ERROR invalid coordinates x=" << x << " y=" << y << endl;
        return false;
    }
    rawdata[(x + (y * width)) * 4] = p.r;
    rawdata[(x + (y * width)) * 4 + 1] = p.g;
    rawdata[(x + (y * width)) * 4 + 2] = p.b;
    rawdata[(x + (y * width)) * 4 + 3] = p.a;
    return true;
}

bool PNG::write(const char * filename)
{
    unsigned error = lodepng::encode(filename, rawdata, width, height);
    if (error)
    {
        cout << __FUNCTION__ << ": ERROR " << lodepng_error_text(error) << endl;
        return false;
    }
    return true;
}

#ifndef PNG_H
#define PNG_H

#include <vector>
#include "pixel.h"

using namespace std;

class PNG {
private:
    unsigned width;
    unsigned height;
    vector<unsigned char> rawdata;

public:

    PNG();

    PNG(const char * filename);

    PNG(PNG & other);

    unsigned getWidth();
    unsigned getHeight();

    Pixel getPixel(unsigned int x, unsigned int y);
    bool setPixel(unsigned int x, unsigned int y, Pixel p);

    bool write(const char * filename);


};

#endif

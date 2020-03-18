#ifndef PIXEL_H
#define PIXEL_H

class Pixel {
private:
    void updateRGB();

    void updateHSL();

public:
    double h, s, l;
    unsigned char r, g, b, a;

    Pixel();

    Pixel(double h, double s, double l);

    Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    bool approximate(const Pixel & p, unsigned int eps);
    bool operator==(const Pixel & p);
    bool operator!=(const Pixel & p);
    Pixel operator=(const Pixel & p);


};

#endif

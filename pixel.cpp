#include <cstdlib>
#include <cmath>
#include "pixel.h"

using namespace std;

Pixel::Pixel() {
    h = 0;
    s = 0;
    l = 0;

    r = 0;
    g = 0;
    b = 0;
    a = 0;
}

// https://www.rapidtables.com/convert/color/hsl-to-rgb.html
void Pixel::updateRGB() {
    double c = (1 - abs(2 * l - 1)) * s;
    double x = c * (1 - abs(fmod(h / 60, 2) - 1));
    double m = l - c / 2;

    double rprime, gprime, bprime;
    if (0 <= h && h < 60) {
        rprime = c;
        gprime = x;
        bprime = 0;
    } else if (60 <= h && h < 120) {
        rprime = x;
        gprime = c;
        bprime = 0;
    } else if (120 <= h && h < 180) {
        rprime = 0;
        gprime = c;
        bprime = x;
    } else if (180 <= h && h < 240) {
        rprime = 0;
        gprime = x;
        bprime = c;
    } else if (240 <= h && h < 300) {
        rprime = x;
        gprime = 0;
        bprime = c;
    } else {
        rprime = c;
        gprime = 0;
        bprime = x;
    }

    r = round((rprime + m) * 255);
    g = round((gprime + m) * 255);
    b = round((bprime + m) * 255);
}

// https://www.rapidtables.com/convert/color/rgb-to-hsl.html
void Pixel::updateHSL() {
    double rprime = ((double)r)/255;
    double gprime = ((double)g)/255;
    double bprime = ((double)b)/255;

    double cmax = fmax(rprime, fmax(gprime, bprime));
    double cmin = fmin(rprime, fmin(gprime, bprime));
    double delt = cmax - cmin;

    l = (cmax + cmin) / 2;

    if (delt < 0.0001) {
        h = 0;
        s = 0;
    } else {
        s = delt / (1 - abs(2 * l - 1));
        if (cmax == rprime) {
            h = 60 * fmod((gprime - bprime) / delt, 6);
        } else if (cmax == gprime) {
            h = 60 * ((bprime - rprime) / delt) + 2;
        } else if (cmax == bprime) {
            h = 60 * ((rprime - gprime) / delt) + 4;
        }
    }
}

Pixel::Pixel(double h, double s, double l) {
    this->h = h;
    this->s = s;
    this->l = l;
    updateRGB();
}

Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
    updateHSL();
}

bool Pixel::approximate(const Pixel & p, unsigned int eps) {
    return (abs(this->r - p.r) <= eps &&
            abs(this->g - p.g) <= eps &&
            abs(this->b - p.b) <= eps &&
            abs(this->a - p.a) <= eps);
}

bool Pixel::operator==(const Pixel & p) {
    return (this->r == p.r &&
            this->g == p.g &&
            this->b == p.b &&
            this->a == p.a);
}

Pixel Pixel::operator=(const Pixel & p) {
    this->r = p.r;
    this->g = p.g;
    this->b = p.b;
    this->a = p.a;
    updateHSL();
    return *this;
}

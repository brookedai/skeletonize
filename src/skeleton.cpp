#include <iostream>
#include "PNG.h"

using namespace std;


int main() {
    cout << "Hello World!" << endl;
    const char *filein = "../images/apple.png";
    const char *fileout = "../out/apple.png";

    PNG img(filein);

    for (unsigned i = 0; i < img.getWidth(); i++) {
        for (unsigned j = 0; j < img.getHeight(); j++) {
            if (img.getPixel(i, j).approximate(Pixel(0,0,0,255), 0)) {
                Pixel p(100, 30, 60, 255);
                img.setPixel(i, j, p);
            }
        }
    }

    img.write(fileout);


    return 0;
}

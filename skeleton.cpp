#include <iostream>
#include "PNG.h"

using namespace std;


int main() {
    cout << "Hello World!" << endl;
    const char *filein = "images/apple.png";
    const char *fileout = "out/apple.png";

    PNG img(filein);

    for (unsigned i = 50; i < 100; i++) {
        for (unsigned j = 50; j < 100; j++) {
            Pixel p(100, 30, 60, 255);
            img.setPixel(i, j, p);
        }
    }

    img.write(fileout);


    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include "PNG.h"

using namespace std;

unsigned int getPixelDistance (unsigned int x,
                               unsigned int y,
                               vector<vector<unsigned int>> & distance_map,
                               PNG & img)
{
    if (x < 0 || x >= img.getWidth() || y < 0 || y >= img.getHeight())
    {
        return 0;
    }
    return distance_map[y][x];
}

void setPixelDistance (unsigned int x,
                       unsigned int y,
                       unsigned int distVal,
                       vector<vector<unsigned int>> & distance_map,
                       PNG & img)
{
    if (x < 0 || x >= img.getWidth() || y < 0 || y >= img.getHeight())
    {
        cout << __FUNCTION__ << ": ERROR could not set pixel distance at x=" << x << " y=" << y << endl;
    }
    distance_map[y][x] = distVal;
}

int main () {
    cout << "Hello World!" << endl;
    const char *filein = "../images/apple.png";
    const char *fileout = "../out/apple.png";

    PNG img(filein);

    vector<vector<unsigned int>> distance_map(img.getHeight(), vector<unsigned int>(img.getWidth()));
    vector<vector<unsigned int>> test(img.getHeight(), vector<unsigned int>(img.getWidth()));

    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            // cout << "forward: " << x << "," << y << endl;
            if (img.getPixel(x, y).approximate(Pixel(0,0,0,255), 100)) {
                unsigned int minNWDist = min(getPixelDistance(x-1, y, distance_map, img) + 1,
                                             getPixelDistance(x, y-1, distance_map, img) + 1);
                setPixelDistance(x, y, minNWDist, distance_map, img);
                setPixelDistance(x, y, 1, test, img);
                Pixel p(100, 30, 60, 255);
                img.setPixel(x, y, p);
            }
        }
    }

    // for (int y = 0; y < img.getHeight(); y++) {
    //     for (int x = 0; x < img.getWidth(); x++) {
    //         if (distance_map[y][x]) cout << distance_map[y][x];
    //         else cout << " ";
    //         cout << ((distance_map[y][x]/100)?" ":(distance_map[y][x]/10)?"  ":"   ");
    //     }
    //     cout << endl;
    // }

    for (int y = img.getHeight()-1; y >= 0; y--) {
        for (int x = img.getWidth()-1; x >= 0; x--) {
            // cout << "backward: " << x << "," << y << endl;
            if (img.getPixel(x, y).approximate(Pixel(100, 30, 60, 255), 100)) {
                unsigned int minSEDist = min(getPixelDistance(x+1, y, distance_map, img) + 1,
                                             min(getPixelDistance(x, y+1, distance_map, img) + 1,
                                                 distance_map[y][x]));
                setPixelDistance(x, y, minSEDist, distance_map, img);
                setPixelDistance(x, y, 1, test, img);
                Pixel p(30, 30, 160, 255);
                img.setPixel(x, y, p);
            }
        }
        // string tmpfile = "../out/apple";
        // tmpfile += to_string(y).c_str();
        // tmpfile += ".png";
        // img.write(tmpfile.c_str());
    }

    for (int y = 0; y < img.getHeight(); y++) {
        for (int x = 0; x < img.getWidth(); x++) {
            // if (distance_map[y][x])
            cout << distance_map[y][x];
            // else
            // cout << " ";
            cout << ((distance_map[y][x]/100)?" ":(distance_map[y][x]/10)?"  ":"   ");
        }
        cout << endl;
    }


    img.write(fileout);


    return 0;
}

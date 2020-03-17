#include <iostream>
#include <vector>
#include <string>
#include "PNG.h"

using namespace std;

unsigned int getPixelDistance (unsigned int x,
                               unsigned int y,
                               vector<vector<unsigned int>> & distance_map)
{
    if (y < 0 || y >= distance_map.size() || x < 0 || x >= distance_map[0].size())
    {
        return 0;
    }
    return distance_map[y][x];
}

void setPixelDistance (unsigned int x,
                       unsigned int y,
                       unsigned int distVal,
                       vector<vector<unsigned int>> & distance_map)
{
    if (y < 0 || y >= distance_map.size() || x < 0 || x >= distance_map[0].size())
    {
        cout << __FUNCTION__ << ": ERROR could not set pixel distance at x=" << x << " y=" << y << endl;
    }
    distance_map[y][x] = distVal;
}

void getDistanceMap (vector<vector<unsigned int>> & distance_map, PNG & img)
{
    distance_map = vector<vector<unsigned int>>(img.getHeight(), vector<unsigned int>(img.getWidth()));

    // start from top-left corner, moving right and down
    for (int y = 0; y < img.getHeight(); y++)
    {
        for (int x = 0; x < img.getWidth(); x++)
        {
            if (img.getPixel(x, y).approximate(Pixel(0,0,0,255), 100))
            {
                unsigned int minTLDist = min(getPixelDistance(x-1, y, distance_map) + 1,
                                             getPixelDistance(x, y-1, distance_map) + 1);
                setPixelDistance(x, y, minTLDist, distance_map);
                Pixel p(100, 30, 60, 255);
                img.setPixel(x, y, p);
            }
        }
    }

    // start from bottom-right corner, moving left and up
    for (int y = img.getHeight()-1; y >= 0; y--)
    {
        for (int x = img.getWidth()-1; x >= 0; x--)
        {
            if (img.getPixel(x, y).approximate(Pixel(100, 30, 60, 255), 100))
            {
                unsigned int minBRDist = min(getPixelDistance(x+1, y, distance_map) + 1,
                                             min(getPixelDistance(x, y+1, distance_map) + 1,
                                                 distance_map[y][x]));
                setPixelDistance(x, y, minBRDist, distance_map);
                Pixel p(30, 30, 160, 255);
                img.setPixel(x, y, p);
            }
        }
    }

}

void getLocalMaxes (vector<vector<unsigned int>> & distance_map,
                    vector<vector<unsigned int>> & local_maxes)
{
    local_maxes = vector<vector<unsigned int>>(distance_map.size(), vector<unsigned int>(distance_map[0].size()));
    for (int y = 0; y < distance_map.size(); y++)
    {
        for (int x = 0; x < distance_map[y].size(); x++)
        {
            if (distance_map[y][x] >= getPixelDistance(x+1,   y, distance_map) &&
                distance_map[y][x] >= getPixelDistance(x+1, y-1, distance_map) &&
                distance_map[y][x] >= getPixelDistance(  x, y-1, distance_map) &&
                distance_map[y][x] >= getPixelDistance(x-1, y-1, distance_map) &&
                distance_map[y][x] >= getPixelDistance(x-1,   y, distance_map) &&
                distance_map[y][x] >= getPixelDistance(x-1, y+1, distance_map) &&
                distance_map[y][x] >= getPixelDistance(  x, y+1, distance_map) &&
                distance_map[y][x] >= getPixelDistance(x+1, y+1, distance_map))
            {
                local_maxes[y][x] = 1;
            }
        }
        cout << endl;
    }
}

void printDistanceMap (vector<vector<unsigned int>> distance_map)
{
    for (int y = 0; y < distance_map.size(); y++)
    {
        for (int x = 0; x < distance_map[y].size(); x++)
        {
            if (distance_map[y][x]) cout << distance_map[y][x];
            else cout << " ";
            cout  << ((distance_map[y][x] / 100) ? " "  :
                                           (distance_map[y][x] /  10) ? "  " :
                                                                        "   ");
        // cout << distance_map[y][x] << ((distance_map[y][x] / 100) ? " "  :
        //                                (distance_map[y][x] /  10) ? "  " :
        //                                                             "   ");
        }
        cout << endl;
    }
}

int main () {
    cout << "Hello World!" << endl;

    vector<const char *> filesin = {"../images/apple.png", "../images/batman.png", "../images/discord.png"};
    vector<const char *> filesout = {"../out/apple.png", "../out/batman.png", "../out/discord.png"};

    for (int i = 0; i < filesin.size(); i++) {
        PNG img(filesin[i]);
        PNG skeleton(filesin[i]);

        vector<vector<unsigned int>> distance_map;
        getDistanceMap(distance_map, img);

        vector<vector<unsigned int>> local_maxes;
        getLocalMaxes(distance_map, local_maxes);

        printDistanceMap(distance_map);
        printDistanceMap(local_maxes);

        img.write(filesout[i]);
    }

    return 0;
}

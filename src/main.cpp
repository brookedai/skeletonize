#include <iostream>
#include <vector>
#include <queue>
#include "PNG.h"
#include "skeleton.h"

using namespace std;

// void recreateImageWithDiskOutlines (vector<vector<int>> & distance_map,
//                                     vector<vector<int>> & ridge_points,
//                                     PNG & img)
// {
//     for (int y = 0; y < distance_map.size(); y++)
//     {
//         for (int x = 0; x < distance_map[y].size(); x++)
//         {
//             if (ridge_points[y][x])
//             {
//                 unsigned char r = (distance_map[y][x] * 20) % 255;
//                 r = (r > 200) ? r : 0;
//                 unsigned char g = 0; //(distance_map[y][x] * 5) % 255;
//                 unsigned char b = (distance_map[y][x] * 35) % 255;//distance_map[y][x] % 255;
//                 b = (b > 200 && r == 0) ? b : 0;
//                 Pixel disk_border((r == 0 && b == 0) ? 255 : r, (r == 0 && b == 0) ? 255 : 0, (r == 0 && b == 0) ? 255 : b, 255);
//                 for (int xoffset = 0; xoffset <= distance_map[y][x]; xoffset++)
//                 {
//                     int yoffset = distance_map[y][x] - xoffset;
//                     if (isPixelValid(x+xoffset, y+yoffset, distance_map))
//                         img.setPixel(x+xoffset, y+yoffset, disk_border);
//                     if (isPixelValid(x+xoffset, y-yoffset, distance_map))
//                         img.setPixel(x+xoffset, y-yoffset, disk_border);
//                     if (isPixelValid(x-xoffset, y+yoffset, distance_map))
//                         img.setPixel(x-xoffset, y+yoffset, disk_border);
//                     if (isPixelValid(x-xoffset, y-yoffset, distance_map))
//                         img.setPixel(x-xoffset, y-yoffset, disk_border);
//
//                 }
//             }
//         }
//     }
// }

void printDistanceMap (vector<vector<int>> & distance_map)
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
    // vector<const char *> filesin = {"../images/batman.png"};
    // vector<const char *> filesout = {"../out/batman.png"};
    vector<const char *> filesin = {"../images/apple.png", "../images/batman.png", "../images/discord.png", "../images/cursive.png", "../images/rose.png", "../images/hansolo.png"};
    vector<const char *> filesout = {"../out/apple.png", "../out/batman.png", "../out/discord.png", "../out/cursive.png", "../out/rose.png", "../out/hansolo.png"};

    for (int i = 0; i < filesin.size(); i++)
    {
        PNG img(filesin[i]);
        Skeleton skeleton(img);
        // vector<vector<int>> distance_map = skeleton.getDistanceMap();
        // printDistanceMap(distance_map);
        skeleton.getRecreatedImage().write(filesout[i]);
    }

    return 0;
}

#include <iostream>
#include <vector>
#include <queue>
#include "PNG.h"
#include "skeleton.h"

using namespace std;

/*
    Prints the distance values of each point, skipping the background points.
    
    @param distance_map The distance map to be printed.
 */
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
        }
        cout << endl;
    }
}

/*
    Driver code for reading PNGs, skeletonizing them, and recreating the image.
*/
int main () {
    vector<const char *> filesin = {"../images/apple.png", "../images/batman.png", "../images/discord.png", "../images/cursive.png", "../images/rose.png", "../images/hansolo.png",
                                    "../images/rectangle.png", "../images/rectangle_border_noise.png", "../images/rectangle_internal_noise.png"};
    vector<const char *> filesout = {"../out/apple.png", "../out/batman.png", "../out/discord.png", "../out/cursive.png", "../out/rose.png", "../out/hansolo.png",
                                    "../out/rectangle.png", "../out/rectangle_border_noise.png", "../out/rectangle_internal_noise.png"};

    for (int i = 0; i < filesin.size(); i++)
    {
        PNG img(filesin[i]);
        Skeleton skeleton(img);
        skeleton.getRecreatedImage().write(filesout[i]);
    }

    return 0;
}

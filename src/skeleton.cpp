#include <iostream>
#include <cstdlib>
#include <queue>
#include "skeleton.h"

#define abs(x) ((x) >= 0 ? (x) : (-x))

#define BLACKPIXEL Pixel(0, 0, 0, 255)
#define WHITEPIXEL Pixel(255, 255, 255, 255)
#define GREYPIXEL Pixel(100, 100, 100, 255)

// private class functions

bool Skeleton::isPixelValid (int x, int y, vector<vector<int>> & v)
{
    return (y >= 0 && y < v.size() && x >= 0 && x < v[0].size());
}

int Skeleton::getPixelDistance (int x, int y)
{
    if (!isPixelValid(x, y, this->distance_map))
        return 0;
    return this->distance_map[y][x];
}

void Skeleton::setPixelDistance (int x, int y, int distVal)
{
    if (!isPixelValid(x, y, this->distance_map))
    {
        cout << __FUNCTION__ << ": ERROR could not set pixel distance at x=" << x << " y=" << y << endl;
        return;
    }
    this->distance_map[y][x] = distVal;
}

void Skeleton::getBinaryImage(PNG & img)
{
    for (int y = 0; y < img.getHeight(); y++)
    {
        for (int x = 0; x < img.getWidth(); x++)
        {
            if (img.getPixel(x, y).approximate(BLACKPIXEL, 100))
            {
                this->binary_img[y][x] = 1;
            }
        }
    }
}

void Skeleton::calculateDistanceMap ()
{
    // start from top-left corner, moving right and down
    for (int y = 0; y < this->binary_img.size(); y++)
    {
        for (int x = 0; x < this->binary_img[y].size(); x++)
        {
            if (this->binary_img[y][x])
            {
                int minTLDist = min(getPixelDistance(x-1, y) + 1,
                                    getPixelDistance(x, y-1) + 1);
                setPixelDistance(x, y, minTLDist);
            }
        }
    }

    // start from bottom-right corner, moving left and up
    for (int y = this->binary_img.size()-1; y >= 0; y--)
    {
        for (int x = this->binary_img[y].size()-1; x >= 0; x--)
        {
            if (this->binary_img[y][x])
            {
                int minBRDist = min(getPixelDistance(x+1, y) + 1,
                                    min(getPixelDistance(x, y+1) + 1,
                                        this->distance_map[y][x]));
                setPixelDistance(x, y, minBRDist);
            }
        }
    }
}

enum prominency {
    NONE,
    STRONG,
    GOOD,
    WEAK
};

void Skeleton::calculateRidgePoints ()
{
    // get the sign of the distance difference between two points
    vector<vector<int>> scanX(this->distance_map.size(),
                              vector<int>(this->distance_map[0].size(), 0));
    vector<vector<int>> scanY(this->distance_map.size(),
                              vector<int>(this->distance_map[0].size(), 0));
    // record each point's likelihood of being a ridge point
    vector<vector<prominency>> ridge_prominency(this->distance_map.size(),
                              vector<prominency>(this->distance_map[0].size(), NONE));

    for (int y = 0; y < this->distance_map.size(); y++)
    {
        for (int x = 0; x < this->distance_map[y].size(); x++)
        {
            scanX[y][x] = getPixelDistance(x, y) - getPixelDistance(x-1,   y);
            scanY[y][x] = getPixelDistance(x, y) - getPixelDistance(  x, y-1);
        }
    }

    for (int y = 0; y < this->distance_map.size(); y++)
    {
        for (int x = 0; x < this->distance_map[y].size(); x++)
        {
            // scanX
            // +0-
            if (isPixelValid(x-1, y, scanX) && isPixelValid(x+1, y, scanX) &&
                scanX[y][x-1] > 0 && scanX[y][x] == 0 && scanX[y][x+1] < 0)
            {
                ridge_prominency[y][x] = STRONG;
            }
            // +-
            else if (isPixelValid(x+1, y, scanX) && scanX[y][x] > 0 && scanX[y][x+1] < 0)
            {
                ridge_prominency[y][x] = STRONG;
            }

            // weak and good prominency points (both scanX and scanY)
            if (ridge_prominency[y][x] == NONE)
            {
                // +0 or 0- on both scan lines
                if (isPixelValid(x+1, y, scanX) &&
                    ((scanX[y][x] > 0 && scanX[y][x+1] == 0) || (scanX[y][x] == 0 && scanX[y][x+1] < 0)) &&
                    isPixelValid(x, y+1, scanY) &&
                    ((scanY[y][x] > 0 && scanY[y+1][x] == 0) || (scanY[y][x] == 0 && scanY[y+1][x] < 0)))
                {
                    ridge_prominency[y][x] = GOOD;
                }
                // +0 or 0- on one scan line
                else if ((isPixelValid(x+1, y, scanX) &&
                    ((scanX[y][x] > 0 && scanX[y][x+1] == 0) || (scanX[y][x] == 0 && scanX[y][x+1] < 0))) ||
                    (isPixelValid(x, y+1, scanY) &&
                    ((scanY[y][x] > 0 && scanY[y+1][x] == 0) || (scanY[y][x] == 0 && scanY[y+1][x] < 0))))
                {
                    ridge_prominency[y][x] = WEAK;
                }
            }

        }
    }

    for (int y = 0; y < this->distance_map.size(); y++)
    {
        for (int x = 0; x < this->distance_map[y].size(); x++)
        {
            // scanY
            // +0-
            if (isPixelValid(x, y-1, scanY) && isPixelValid(x, y+1, scanY) &&
                scanY[y-1][x] > 0 && scanY[y][x] == 0 && scanY[y+1][x] < 0)
            {
                if (ridge_prominency[y+1][x] == STRONG) continue; // removes pixels that are off by one
                ridge_prominency[y][x] = STRONG;
            }
            // +-
            else if (isPixelValid(x, y+1, scanY) && scanY[y][x] > 0 && scanY[y+1][x] < 0)
            {
                ridge_prominency[y][x] = STRONG;
            }
        }
    }

    vector<vector<bool>> visited(this->ridge_points.size(), vector<bool>(this->ridge_points[0].size()));
    // first pass
    for (int y = 0; y < ridge_prominency.size(); y++)
    {
        for (int x = 0; x < ridge_prominency[y].size(); x++)
        {
            if (ridge_prominency[y][x] == STRONG || ridge_prominency[y][x] == GOOD)
            {
                this->ridge_points[y][x] = ridge_prominency[y][x];
                visited[y][x] = true;
            }

        }
    }

    // second pass
    for (int y = 0; y < ridge_prominency.size(); y++)
    {
        for (int x = 0; x < ridge_prominency[y].size(); x++)
        {
            if (this->ridge_points[y][x] == NONE) continue;
            int countNeighbours = 0;
            int dx = 0; // deltas determine which direction we should go when
            int dy = 0; // seeking new points
            if (isPixelValid(x+1, y, this->ridge_points) && this->ridge_points[y][x+1] != NONE)
            {
                countNeighbours++;
                dx = -1;
            }
            if (isPixelValid(x, y-1, this->ridge_points) && this->ridge_points[y-1][x] != NONE)
            {
                countNeighbours++;
                dy = 1;
            }
            if (isPixelValid(x-1, y, this->ridge_points) && this->ridge_points[y][x-1] != NONE)
            {
                countNeighbours++;
                dx = 1;
            }
            if (isPixelValid(x, y+1, this->ridge_points) && this->ridge_points[y+1][x] != NONE)
            {
                countNeighbours++;
                dy = -1;
            }
            if (isPixelValid(x+1, y-1, this->ridge_points) && this->ridge_points[y-1][x+1] != NONE)
            {
                countNeighbours++;
                dx = -1;
                dy = 1;

            }
            if (isPixelValid(x-1, y-1, this->ridge_points) && this->ridge_points[y-1][x-1] != NONE)
            {
                countNeighbours++;
                dx = 1;
                dy = 1;
            }
            if (isPixelValid(x-1, y+1, this->ridge_points) && this->ridge_points[y+1][x-1] != NONE)
            {
                countNeighbours++;
                dx = 1;
                dy = -1;
            }
            if (isPixelValid(x+1, y+1, this->ridge_points) && this->ridge_points[y+1][x+1])
            {
                countNeighbours++;
                dx = -1;
                dy = -1;
            }

            if (countNeighbours < 2)
            {
                // cout << "neighbours=" << countNeighbours << " x=" << x << " y=" << y << " prominence=" << ridge_prominency[y][x] << endl;
                int currx = x;
                int curry = y;
                vector<pair<int,int>> tentative;
                while (isPixelValid(currx, curry, this->ridge_points))
                {
                    int tmpx = currx;
                    int tmpy = curry;
                    // weak point
                    if (dx && isPixelValid(currx+dx, curry, this->ridge_points) && ridge_prominency[curry][currx+dx] != NONE)
                    {
                        tmpx = currx+dx;
                    }
                    // weak point
                    if (dy && isPixelValid(currx, curry+dy, this->ridge_points) && ridge_prominency[curry+dy][currx] != NONE)
                    {
                        tmpy = curry+dy;
                    }
                    // no weak points around
                    if (currx == tmpx && curry == tmpy)
                    {
                        int currMax = 0;
                        // check every neighbour
                        if (isPixelValid(currx+1, curry, this->ridge_points) &&
                            this->ridge_points[curry][currx+1] == NONE &&
                            abs(distance_map[curry][currx+1]) > currMax)
                        {
                            currMax = abs(distance_map[curry][currx+1]);
                            tmpx = currx+1;
                            tmpy = curry;
                        }
                        if (isPixelValid(currx-1, curry, this->ridge_points) &&
                            this->ridge_points[curry][currx-1] == NONE &&
                            abs(distance_map[curry][currx-1]) > currMax)
                        {
                            currMax = abs(distance_map[curry][currx-1]);
                            tmpx = currx-1;
                            tmpy = curry;
                        }
                        if (isPixelValid(currx, curry+1, this->ridge_points) &&
                            this->ridge_points[curry+1][currx] == NONE &&
                            abs(distance_map[curry+1][currx]) > currMax)
                        {
                            currMax = abs(distance_map[curry+1][currx]);
                            tmpx = currx;
                            tmpy = curry+1;
                        }
                        if (isPixelValid(currx, curry-1, this->ridge_points) &&
                            this->ridge_points[curry-1][currx] == NONE &&
                            abs(distance_map[curry-1][currx]) > currMax)
                        {
                            currMax = abs(distance_map[curry-1][currx]);
                            tmpx = currx;
                            tmpy = curry-1;
                        }
                        if (isPixelValid(currx+1, curry+1, this->ridge_points) &&
                            this->ridge_points[curry+1][currx+1] == NONE &&
                            abs(distance_map[curry+1][currx+1]) > currMax)
                        {
                            currMax = abs(distance_map[curry+1][currx+1]);
                            tmpx = currx+1;
                            tmpy = curry+1;
                        }
                        if (isPixelValid(currx-1, curry-1, this->ridge_points) &&
                            this->ridge_points[curry-1][currx-1] == NONE &&
                            abs(distance_map[curry-1][currx-1]) > currMax)
                        {
                            currMax = abs(distance_map[curry-1][currx-1]);
                            tmpx = currx-1;
                            tmpy = curry-1;
                        }
                        if (isPixelValid(currx+1, curry-1, this->ridge_points) &&
                            this->ridge_points[curry-1][currx+1] == NONE &&
                            abs(distance_map[curry-1][currx+1]) > currMax)
                        {
                            currMax = abs(distance_map[curry-1][currx+1]);
                            tmpx = currx+1;
                            tmpy = curry-1;
                        }
                        if (isPixelValid(currx-1, curry+1, this->ridge_points) &&
                            this->ridge_points[curry+1][currx-1] == NONE &&
                            abs(distance_map[curry+1][currx-1]) > currMax)
                        {
                            currMax = abs(distance_map[curry+1][currx-1]);
                            tmpx = currx-1;
                            tmpy = curry+1;
                        }
                    }
                    // bump into an invalid position
                    if (currx == tmpx && curry == tmpy)
                    {
                        currx = -1;
                        curry = -1;
                        break;
                    }

                    currx = tmpx;
                    curry = tmpy;
                    if (isPixelValid(tmpx, tmpy, this->ridge_points) && this->ridge_points[tmpy][tmpx] != NONE) break;
                    if (visited[curry][currx]) break;
                    visited[curry][currx] = true;
                    tentative.push_back({currx, curry});

                    // has two neighbouring points on the ridge line
                    int currNeighbourCount = 0;
                    if (isPixelValid(currx-1, curry, this->ridge_points) &&
                        visited[curry][currx-1])
                        // this->ridge_points[curry][currx-1] != NONE)
                        currNeighbourCount++;
                    if (isPixelValid(currx+1, curry, this->ridge_points) &&
                        visited[curry][currx+1])
                        // this->ridge_points[curry][currx+1] != NONE)
                        currNeighbourCount++;
                    if (isPixelValid(currx, curry-1, this->ridge_points) &&
                        visited[curry-1][currx])
                        // this->ridge_points[curry-1][currx] != NONE)
                        currNeighbourCount++;
                    if (isPixelValid(currx, curry+1, this->ridge_points) &&
                        visited[curry+1][currx])
                        // this->ridge_points[curry+1][currx] != NONE)
                        currNeighbourCount++;
                    if (currNeighbourCount >= 2)
                    {
                        break;
                    }
                }

                // invalid branch - off the shape or didn't collide with established skeleton
                if (!isPixelValid(currx, curry, this->ridge_points) || this->distance_map[curry][currx] == 0)
                {
                    // cout << !isPixelValid(currx, curry, this->ridge_points);
                    // if (isPixelValid(currx, curry, this->ridge_points)) cout << " " << (this->distance_map[curry][currx] == 0) << " " << (this->ridge_points[curry][currx] == NONE) << endl;
                    // else cout << endl;
                    continue;
                }

                for (pair<int,int> coord : tentative)
                {
                    int x = coord.first;
                    int y = coord.second;
                    if (this->ridge_points[y][x] == NONE) this->ridge_points[y][x] = WEAK;
                }
            }
        }
    }
}

void Skeleton::recreateImage ()
{
    if (this->distance_map.size() == 0 ||
        this->distance_map.size() != this->ridge_points.size() ||
        this->distance_map[0].size() != this->ridge_points[0].size())
    {
        cout << __FUNCTION__ << ": ERROR distance map and local maxes size mismatch" << endl;
        return;
    }

    vector<vector<vector<bool>>> visited(this->distance_map.size(),
                                         vector<vector<bool>>(this->distance_map[0].size(),
                                         vector<bool>(this->distance_map.size() + this->distance_map[0].size())));

    for (int y = 0; y < this->distance_map.size(); y++)
    {
        for (int x = 0; x < this->distance_map[y].size(); x++)
        {
            if (this->ridge_points[y][x] == STRONG || this->ridge_points[y][x] == GOOD || this->ridge_points[y][x] == WEAK)
            {
                // cout << "===================processing " << x << "," << y << endl;
                // (x, y), number of steps left
                queue<pair<pair<int, int>, int>> pixel_queue;
                pixel_queue.push({{x, y}, this->distance_map[y][x]});
                if (this->ridge_points[y][x] == STRONG) this->recreated_img.setPixel(x, y, Pixel(0, 255, 0, 255));
                else if (this->ridge_points[y][x] == GOOD) this->recreated_img.setPixel(x, y, Pixel(0, 0, 255, 255));
                else if (this->ridge_points[y][x] == WEAK) this->recreated_img.setPixel(x, y, Pixel(255, 0, 0, 255));

                while (!pixel_queue.empty())
                {
                    int currx, curry;
                    int steps;
                    pair<int, int> coordinates;
                    tie(coordinates, steps) = pixel_queue.front();
                    tie(currx, curry) = coordinates;
                    pixel_queue.pop();
                    if (visited[curry][currx][steps]) continue;
                    visited[curry][currx][steps] = true;
                    // cout << currx << "," << curry << " steps: " << steps << endl;

                    if (this->recreated_img.getPixel(currx, curry) == WHITEPIXEL)
                    {
                        this->recreated_img.setPixel(currx, curry, GREYPIXEL);
                    }

                    if (steps != 0)
                    {
                        if (isPixelValid(currx+1, curry, distance_map) && !visited[curry][currx+1][steps-1])
                        {
                            pixel_queue.push({{currx+1, curry}, steps-1});
                        }
                        if (isPixelValid(currx, curry-1, distance_map) && !visited[curry-1][currx][steps-1])
                        {
                            pixel_queue.push({{currx, curry-1}, steps-1});
                        }
                        if (isPixelValid(currx-1, curry, distance_map) && !visited[curry][currx-1][steps-1])
                        {
                            pixel_queue.push({{currx-1, curry}, steps-1});
                        }
                        if (isPixelValid(currx, curry+1, distance_map) && !visited[curry+1][currx][steps-1])
                        {
                            pixel_queue.push({{currx, curry+1}, steps-1});
                        }
                    }
                }
            }
        }
    }
}

// public class functions

Skeleton::Skeleton ()
{
    // cannot initialize anything
}

Skeleton::Skeleton (int width, int height)
{
    if (width == 0 || height == 0)
    {
        cout << __FUNCTION__ << ": ERROR could not initialize skeleton with width=" << width << " and height=" << height << endl;
        return;
    }
    this->binary_img = vector<vector<int>>(height, vector<int>(width, 0));
    this->distance_map = vector<vector<int>>(height, vector<int>(width, 0));
    this->ridge_points = vector<vector<int>>(height, vector<int>(width, 0));
    this->recreated_img = PNG(width, height);
}

Skeleton::Skeleton (vector<vector<int>> & img)
{
    if (img.size() == 0 || img[0].size() == 0)
    {
        cout << __FUNCTION__ << ": ERROR could not initialize skeleton with given image vector" << endl;
        return;
    }
    this->binary_img = img;
    this->distance_map = vector<vector<int>>(img.size(), vector<int>(img[0].size(), 0));
    this->ridge_points = vector<vector<int>>(img.size(), vector<int>(img[0].size(), 0));
    this->recreated_img = PNG(img[0].size(), img.size());

    calculateDistanceMap();
    calculateRidgePoints();
    recreateImage();
}

Skeleton::Skeleton (PNG & img)
{
    if (img.getWidth() == 0 || img.getHeight() == 0)
    {
        cout << __FUNCTION__ << ": ERROR could not initialize skeleton with given image" << endl;
        return;
    }
    this->img = img;
    this->binary_img = vector<vector<int>>(img.getHeight(), vector<int>(img.getWidth(), 0));
    getBinaryImage (img);
    this->distance_map = vector<vector<int>>(img.getHeight(), vector<int>(img.getWidth(), 0));
    this->ridge_points = vector<vector<int>>(img.getHeight(), vector<int>(img.getWidth(), 0));
    this->recreated_img = PNG(img.getWidth(), img.getHeight());

    calculateDistanceMap();
    calculateRidgePoints();
    recreateImage();
}

vector<vector<int>> Skeleton::getDistanceMap ()
{
    return this->distance_map;
}

vector<vector<int>> Skeleton::getRidgePoints ()
{
    return this->ridge_points;
}

PNG Skeleton::getRecreatedImage ()
{
    return this->recreated_img;
}

#include <iostream>
#include <cstdlib>
#include <queue>
#include <functional>
#include <algorithm>
#include "skeleton.h"

#define abs(x) ((x) >= 0 ? (x) : (-x))

#define BLACKPIXEL Pixel(0, 0, 0, 255)
#define WHITEPIXEL Pixel(255, 255, 255, 255)
#define GREYPIXEL Pixel(100, 100, 100, 255)

/*
    ============================================================================
    ======================= PRIVATE CLASS FUNCTIONS ============================
    ============================================================================
*/

/*
    Checks whether the coordinates are valid in a given vector.

    @param x The x coordinate (second index of the vector)
    @param y The y coordinate (first index of the vector)
    @param v The vector to check the coordinates' validity against
*/
bool Skeleton::isPixelValid (int x, int y, vector<vector<int>> & v)
{
    return (y >= 0 && y < v.size() && x >= 0 && x < v[0].size());
}

/*
    Returns the pixel's distance value in the distance map,
    returns 0 if the pixel is invalid.

    @param x The x coordinate of the pixel
    @param y The y coordinate of the pixel
    @return The distance value of the pixel at (x, y)
*/
int Skeleton::getPixelDistance (int x, int y)
{
    if (!isPixelValid(x, y, this->distance_map))
        return 0;
    return this->distance_map[y][x];
}

/*
    Sets the pixel's distance value in the distance map,
    does nothing if the pixel's coordinates are invalid

    @param x The x coordinate of the pixel
    @param y The y coordinate of the pixel
    @param distVal The distance value to be set
*/
void Skeleton::setPixelDistance (int x, int y, int distVal)
{
    if (!isPixelValid(x, y, this->distance_map))
    {
        cout << __FUNCTION__ << ": ERROR could not set pixel distance at x=" << x << " y=" << y << endl;
        return;
    }
    this->distance_map[y][x] = distVal;
}


/*
    Initializes the binary_img vector with the given PNG.
    The PNG must have black pixels representing the shape, and
    white pixels representing the background.

    @param img The png image used to initialize binary_img
*/
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

/*
    Initializes the distance_map vector with distance values
    of each pixel (Manhattan distance to the border).

    Precondition: The distance_map vector is initialized
    to be a height x width 2d vector, with 0 as the default
    value.
*/
void Skeleton::calculateDistanceMap ()
{
    // start from top-left corner, moving right and down
    for (int y = 0; y < this->binary_img.size(); y++)
    {
        for (int x = 0; x < this->binary_img[y].size(); x++)
        {
            if (this->binary_img[y][x])
            {
                // take the minimum of the left and top neighbours + 1
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
                // take the minimum of the right and bottom neighbours + 1
                // and the current distance value
                int minBRDist = min(getPixelDistance(x+1, y) + 1,
                                    min(getPixelDistance(x, y+1) + 1,
                                        this->distance_map[y][x]));
                setPixelDistance(x, y, minBRDist);
            }
        }
    }
}

// labels for the candidate ridge points
enum prominency {
    NONE,
    WEAK,
    GOOD,
    STRONG
};

/*
    Helper function to calculate the transition values between points,
    eg. We have two pixels | 1 | 2 | and we are scanning in the x direction.
        Then, the value of scanX for the pixel with distance value 2 is 1,
        and that positive value would contribute a + to the pattern finding
        in the labelling step.

    @param scanX The scan map for the scan line going in the x direction
    @param scanY The scan map for the scan line going in the y direction
*/
static void calculateScanMap (vector<vector<int>> &scanX,
                              vector<vector<int>> &scanY)
{
    for (int y = 0; y < this->distance_map.size(); y++)
    {
        for (int x = 0; x < this->distance_map[y].size(); x++)
        {
            scanX[y][x] = getPixelDistance(x, y) - getPixelDistance(x-1,   y);
            scanY[y][x] = getPixelDistance(x, y) - getPixelDistance(  x, y-1);
        }
    }
}

/*
    Helper function to label ridge candidate points by how strong of a ridge
    indicator it is.
    The four relevant patterns, or prominent sign barriers, are +-, +0-, +0,
    and 0-.

    STRONG: +- and +0-
    GOOD: +0 or -0 on both scan lines
    WEAK: +0 or -0 on one scan line
    NONE: does not match any of the four patterns above.


*/
static void labelCandidates (vector<vector<int>> &scanX,
                             vector<vector<int>> &scanY,
                             vector<vector<prominency>> &ridge_prominency)
{
    // the STRONG labelling for scanX, as well as the GOOD and WEAK labelling
    // for both scanX and scanY, are done in this double for loop.
    for (int y = 0; y < this->distance_map.size(); y++)
    {
        for (int x = 0; x < this->distance_map[y].size(); x++)
        {
            // scanX STRONG labels
            // +0-
            if (isPixelValid(x-1, y, scanX) && isPixelValid(x+1, y, scanX) &&
                scanX[y][x-1] > 0 && scanX[y][x] == 0 && scanX[y][x+1] < 0)
            {
                ridge_prominency[y][x] = STRONG;
            }
            // +-
            else if (isPixelValid(x+1, y, scanX) && scanX[y][x] > 0 &&
                     scanX[y][x+1] < 0)
            {
                ridge_prominency[y][x] = STRONG;
            }

            // WEAK and GOOD prominency points (both scanX and scanY)
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

    // now the STRONG labelling for scanY. this must be separated from the first
    // double for loop because all the STRONG points on the x scan line must
    // be processed first to produce a cleaner skeleton.
    // otherwise, if two seemingly equally valid STRONG points are presented,
    // a redundant one may be chosen.
    // this only occurs in the +0- case.
    for (int y = 0; y < this->distance_map.size(); y++)
    {
        for (int x = 0; x < this->distance_map[y].size(); x++)
        {
            // scanY STRONG labels
            // +0-
            if (isPixelValid(x, y-1, scanY) && isPixelValid(x, y+1, scanY) &&
                scanY[y-1][x] > 0 && scanY[y][x] == 0 && scanY[y+1][x] < 0)
            {
                // checks whether the other point has already been chosen,
                // skip the current one if it has
                if (ridge_prominency[y+1][x] == STRONG) continue;
                ridge_prominency[y][x] = STRONG;
            }
            // +-
            else if (isPixelValid(x, y+1, scanY) && scanY[y][x] > 0 && scanY[y+1][x] < 0)
            {
                ridge_prominency[y][x] = STRONG;
            }
        }
    }
}

/*
    First pass to choose all the ridge point candidates that have the label
    STRONG or GOOD.

    @param ridge_prominency The label values for each point
    @param visited The points already considered for the skeleton
*/
static void ridgePointsFirstPass (vector<vector<prominency>> &ridge_prominency,
                                  vector<vector<bool>> &visited)
{
    for (int y = 0; y < ridge_prominency.size(); y++)
    {
        for (int x = 0; x < ridge_prominency[y].size(); x++)
        {
            // add all the points labelled STRONG and GOOD to the skeleton
            if (ridge_prominency[y][x] == STRONG ||
                ridge_prominency[y][x] == GOOD)
            {
                this->ridge_points[y][x] = ridge_prominency[y][x];
                visited[y][x] = true;
            }
        }
    }
}

/*
    Second pass to link disconnected segments of the skeleton by choosing
    WEAK points or, if no WEAK points are available, points with the greatest
    distance value.

    WARNING: this function is a monstrosity.
*/
static void ridgePointsSecondPass (vector<vector<prominency>> &ridge_prominency,
                                   vector<vector<bool>> &visited)
{
    // iterate through all points to find the points already in the skeleton
    for (int y = 0; y < ridge_prominency.size(); y++)
    {
        for (int x = 0; x < ridge_prominency[y].size(); x++)
        {
            if (this->ridge_points[y][x] == NONE) continue;
            int countNeighbours = 0;
            int dx = 0; // deltas determine which direction we should go when
            int dy = 0; // seeking new points

            // counting the number of neighbours the point currently has
            // (8 neighbours)
            if (isPixelValid(x+1, y, this->ridge_points) && // right
                this->ridge_points[y][x+1] != NONE)
            {
                countNeighbours++;
                dx = -1;
            }
            if (isPixelValid(x, y-1, this->ridge_points) && // top
                this->ridge_points[y-1][x] != NONE)
            {
                countNeighbours++;
                dy = 1;
            }
            if (isPixelValid(x-1, y, this->ridge_points) && // left
                this->ridge_points[y][x-1] != NONE)
            {
                countNeighbours++;
                dx = 1;
            }
            if (isPixelValid(x, y+1, this->ridge_points) && // bottom
                this->ridge_points[y+1][x] != NONE)
            {
                countNeighbours++;
                dy = -1;
            }
            if (isPixelValid(x+1, y-1, this->ridge_points) && // upper right
                this->ridge_points[y-1][x+1] != NONE)
            {
                countNeighbours++;
                dx = -1;
                dy = 1;

            }
            if (isPixelValid(x-1, y-1, this->ridge_points) && // upper left
                this->ridge_points[y-1][x-1] != NONE)
            {
                countNeighbours++;
                dx = 1;
                dy = 1;
            }
            if (isPixelValid(x-1, y+1, this->ridge_points) && // lower left
                this->ridge_points[y+1][x-1] != NONE)
            {
                countNeighbours++;
                dx = 1;
                dy = -1;
            }
            if (isPixelValid(x+1, y+1, this->ridge_points) && // lower right
                this->ridge_points[y+1][x+1])
            {
                countNeighbours++;
                dx = -1;
                dy = -1;
            }

            // extend tentative branch if point has less than 2 neighbours
            if (countNeighbours < 2)
            {
                // coordinates of current point on the tentative branch
                int currx = x;
                int curry = y;
                vector<pair<int,int>> tentative; // stores all points in
                                                 // tentative branch
                // keep extending the branch until the pixel is invalid
                // (fail case; there are breaks in the middle for the success case)
                while (isPixelValid(currx, curry, this->ridge_points))
                {
                    // coordinates of the next point to go to
                    int tmpx = currx;
                    int tmpy = curry;

                    // weak point moving in x direction
                    if (dx && isPixelValid(currx+dx, curry, this->ridge_points) &&
                        ridge_prominency[curry][currx+dx] != NONE)
                    {
                        tmpx = currx+dx;
                    }
                    // weak point moving in y direction
                    if (dy && isPixelValid(currx, curry+dy, this->ridge_points) &&
                        ridge_prominency[curry+dy][currx] != NONE)
                    {
                        tmpy = curry+dy;
                    }
                    // no weak points around, find point with max distance val
                    if (currx == tmpx && curry == tmpy)
                    {
                        int currMax = 0;
                        // check every neighbour, skip the ones already in the
                        // skeleton
                        if (isPixelValid(currx+1, curry, this->ridge_points) && // right
                            this->ridge_points[curry][currx+1] == NONE &&
                            abs(distance_map[curry][currx+1]) > currMax)
                        {
                            currMax = abs(distance_map[curry][currx+1]);
                            tmpx = currx+1;
                            tmpy = curry;
                        }
                        if (isPixelValid(currx-1, curry, this->ridge_points) && // left
                            this->ridge_points[curry][currx-1] == NONE &&
                            abs(distance_map[curry][currx-1]) > currMax)
                        {
                            currMax = abs(distance_map[curry][currx-1]);
                            tmpx = currx-1;
                            tmpy = curry;
                        }
                        if (isPixelValid(currx, curry+1, this->ridge_points) && // bottom
                            this->ridge_points[curry+1][currx] == NONE &&
                            abs(distance_map[curry+1][currx]) > currMax)
                        {
                            currMax = abs(distance_map[curry+1][currx]);
                            tmpx = currx;
                            tmpy = curry+1;
                        }
                        if (isPixelValid(currx, curry-1, this->ridge_points) && // top
                            this->ridge_points[curry-1][currx] == NONE &&
                            abs(distance_map[curry-1][currx]) > currMax)
                        {
                            currMax = abs(distance_map[curry-1][currx]);
                            tmpx = currx;
                            tmpy = curry-1;
                        }
                        if (isPixelValid(currx+1, curry+1, this->ridge_points) && // bottom right
                            this->ridge_points[curry+1][currx+1] == NONE &&
                            abs(distance_map[curry+1][currx+1]) > currMax)
                        {
                            currMax = abs(distance_map[curry+1][currx+1]);
                            tmpx = currx+1;
                            tmpy = curry+1;
                        }
                        if (isPixelValid(currx-1, curry-1, this->ridge_points) && // top left
                            this->ridge_points[curry-1][currx-1] == NONE &&
                            abs(distance_map[curry-1][currx-1]) > currMax)
                        {
                            currMax = abs(distance_map[curry-1][currx-1]);
                            tmpx = currx-1;
                            tmpy = curry-1;
                        }
                        if (isPixelValid(currx+1, curry-1, this->ridge_points) && // top right
                            this->ridge_points[curry-1][currx+1] == NONE &&
                            abs(distance_map[curry-1][currx+1]) > currMax)
                        {
                            currMax = abs(distance_map[curry-1][currx+1]);
                            tmpx = currx+1;
                            tmpy = curry-1;
                        }
                        if (isPixelValid(currx-1, curry+1, this->ridge_points) && // bottom left
                            this->ridge_points[curry+1][currx-1] == NONE &&
                            abs(distance_map[curry+1][currx-1]) > currMax)
                        {
                            currMax = abs(distance_map[curry+1][currx-1]);
                            tmpx = currx-1;
                            tmpy = curry+1;
                        }
                    }

                    // if no changes to tmp coordinates, we have no more valid
                    // points to go to: bump into an invalid position and fail
                    if (currx == tmpx && curry == tmpy)
                    {
                        currx = -1;
                        curry = -1;
                        break;
                    }

                    // set current coordinates to next coordinates
                    currx = tmpx;
                    curry = tmpy;

                    // if the current point is already visited, fail
                    if (visited[curry][currx]) break;
                    visited[curry][currx] = true;

                    // add coordinates to the tentative branch
                    tentative.push_back({currx, curry});

                    // check if current point has two or more neighbouring points
                    // on the ridge line; if so, break with success
                    int currNeighbourCount = 0;
                    if (isPixelValid(currx-1, curry, this->ridge_points) &&
                        visited[curry][currx-1])
                        currNeighbourCount++;
                    if (isPixelValid(currx+1, curry, this->ridge_points) &&
                        visited[curry][currx+1])
                        currNeighbourCount++;
                    if (isPixelValid(currx, curry-1, this->ridge_points) &&
                        visited[curry-1][currx])
                        currNeighbourCount++;
                    if (isPixelValid(currx, curry+1, this->ridge_points) &&
                        visited[curry+1][currx])
                        currNeighbourCount++;

                    if (currNeighbourCount >= 2) break;
                } // end of extending tentative branch; now we decide whether
                  // to keep it


                // invalid branch - off the shape or didn't collide with established skeleton
                if (!isPixelValid(currx, curry, this->ridge_points) ||
                    this->distance_map[curry][currx] == 0)
                {
                    continue;
                }

                // success - add all branch points to the skeleton
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

/*
    Main function to find skeleton. This function first calculates the values
    for the x and y scan maps, then uses the scan map values to label each
    point based on how strong of a ridge indicator it is. Then, it does two
    scans to add the appropriate points to the skeleton (ridge_points).
*/
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

    // processes all values for both scan maps
    calculateScanMap(scanX, scanY);

    // labels the ridge point candidates
    labelCandidates(scanX, scanY, ridge_prominency);

    // visited vector prevents the algorithm from choosing points that go in a
    // circle forever
    vector<vector<bool>> visited(this->ridge_points.size(),
                                 vector<bool>(this->ridge_points[0].size()));

    // first pass
    ridgePointsFirstPass(ridge_prominency, visited);

    // second pass
    ridgePointsSecondPass(ridge_prominency, visited);
}

/*
    Recreates the image based on the skeleton of the image and the distance map.
*/
void Skeleton::recreateImage ()
{
    if (this->distance_map.size() == 0 ||
        this->distance_map.size() != this->ridge_points.size() ||
        this->distance_map[0].size() != this->ridge_points[0].size())
    {
        cout << __FUNCTION__ << ": ERROR distance map and local maxes size mismatch" << endl;
        return;
    }

    // visited[y][x][d], tracks if we've already processed the point (x, y)
    // with remaining steps d
    vector<vector<vector<bool>>> visited(this->distance_map.size(),
                                         vector<vector<bool>>(this->distance_map[0].size(),
                                         vector<bool>(this->distance_map.size() + this->distance_map[0].size())));

    for (int y = 0; y < this->distance_map.size(); y++)
    {
        for (int x = 0; x < this->distance_map[y].size(); x++)
        {
            // checking if current point is part of the skeleton
            if (this->ridge_points[y][x] != NONE)
            {
                // stores (x, y) with remaining steps d
                queue<pair<pair<int, int>, int>> pixel_queue;
                pixel_queue.push({{x, y}, this->distance_map[y][x]});

                // setting the colour of the center of the circle
                // sets STRONG pixels to green
                if (this->ridge_points[y][x] == STRONG)
                    this->recreated_img.setPixel(x, y, Pixel(0, 255, 0, 255));
                // sets GOOD pixels to blue
                else if (this->ridge_points[y][x] == GOOD)
                    this->recreated_img.setPixel(x, y, Pixel(0, 0, 255, 255));
                // sets WEAK pixels to red
                else if (this->ridge_points[y][x] == WEAK)
                    this->recreated_img.setPixel(x, y, Pixel(255, 0, 0, 255));

                // colour in the circle centered at the current point with
                // radius equal to the distance function at this point
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

                    // if the pixel isn't set yet, set it to grey
                    if (this->recreated_img.getPixel(currx, curry) == WHITEPIXEL)
                    {
                        this->recreated_img.setPixel(currx, curry, GREYPIXEL);
                    }

                    // if we still need to keep going, check in all four directions
                    if (steps != 0)
                    {
                        if (isPixelValid(currx+1, curry, distance_map) &&
                            !visited[curry][currx+1][steps-1])
                        {
                            pixel_queue.push({{currx+1, curry}, steps-1});
                        }
                        if (isPixelValid(currx, curry-1, distance_map) &&
                            !visited[curry-1][currx][steps-1])
                        {
                            pixel_queue.push({{currx, curry-1}, steps-1});
                        }
                        if (isPixelValid(currx-1, curry, distance_map) &&
                            !visited[curry][currx-1][steps-1])
                        {
                            pixel_queue.push({{currx-1, curry}, steps-1});
                        }
                        if (isPixelValid(currx, curry+1, distance_map) &&
                            !visited[curry+1][currx][steps-1])
                        {
                            pixel_queue.push({{currx, curry+1}, steps-1});
                        }
                    }
                }
            }
        }
    }
}

/*
    ============================================================================
    ========================= PUBLIC CLASS FUNCTIONS ===========================
    ============================================================================
*/

/*
    Default constructor for a Skeleton object.
    (kind of useless)
*/
Skeleton::Skeleton ()
{
    // cannot initialize anything
}

/*
    Constructor for a skeleton for an image's width and height.

    @param width The width of the image
    @param height The height of the image
    (also kind of useless)
*/
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

/*
    Constructor for a skeleton given a 2d vector of pixel values.

    @param img The 2d vector representing a binary image, with
               values 0 and 1
*/
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
    // cout << "recreated image size: " << this->recreated_img.getWidth() << " " << this->recreated_img.getHeight() << endl;

    calculateDistanceMap();
    calculateRidgePoints();
    recreateImage();
}

/*
    Constructor for a skeleton given a png image.

    @param img The binary png image
*/
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

/*
    Returns the distance map.
    (does not calculate the distance values)

    @return the distance map of the skeleton
*/
vector<vector<int>> Skeleton::getDistanceMap ()
{
    return this->distance_map;
}

/*
    Returns a vector with the ridge points as non-zero values.
    (does not calculate the ridge points)

    @return vector with ridge points as non-zero values
*/
vector<vector<int>> Skeleton::getRidgePoints ()
{
    return this->ridge_points;
}

/*
    Returns the recreated image.
    (does not calculate the recreated image)

    @return the recreated image as a png
*/
PNG Skeleton::getRecreatedImage ()
{
    return this->recreated_img;
}

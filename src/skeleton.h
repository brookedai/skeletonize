#ifndef SKELETON_H
#define SKELETON_H

#include <vector>
#include "PNG.h"

using namespace std;

// labels for the candidate ridge points
enum prominency {
    NONE,
    WEAK,
    GOOD,
    STRONG
};

class Skeleton {
private:
    PNG img;
    vector<vector<int>> binary_img;
    vector<vector<int>> distance_map;
    vector<vector<int>> ridge_points;
    PNG recreated_img;

    bool isPixelValid (int x, int y, vector<vector<int>> & v);

    int getPixelDistance (int x, int y);

    void setPixelDistance (int x, int y, int distVal);

    void getBinaryImage (PNG & img);

    void calculateDistanceMap ();

    void calculateScanMap (vector<vector<int>> &scanX, vector<vector<int>> &scanY);

    void labelCandidates (vector<vector<int>> &scanX,
                          vector<vector<int>> &scanY,
                          vector<vector<prominency>> &ridge_prominency);

    void ridgePointsFirstPass (vector<vector<prominency>> &ridge_prominency,
                               vector<vector<bool>> &visited);

    void ridgePointsSecondPass (vector<vector<prominency>> &ridge_prominency,
                                vector<vector<bool>> &visited);

    void calculateRidgePoints ();

    void recreateImage ();

public:
    Skeleton ();

    Skeleton (int width, int height);

    Skeleton (vector<vector<int>> & img);

    Skeleton (PNG & img);

    vector<vector<int>> getDistanceMap ();

    vector<vector<int>> getRidgePoints ();

    PNG getRecreatedImage ();

};

#endif

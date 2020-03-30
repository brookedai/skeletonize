#include <iostream>
#include <vector>
#include "PNG.h"
#include "skeleton.h"

#define WHITEPIXEL Pixel(255, 255, 255, 255)

using namespace std;

struct data_tuple {
    int x, y, d;
    data_tuple(int x, int y, int d) {
        this->x = x;
        this->y = y;
        this->d = d;
    }
};

int W, L;
vector<vector<int>> img;
Skeleton skeleton;

/*
    Helper function to take in the problem input and produce the solution.
    Initializes the golbal variables.
*/
void solveProblem() {
    cin >> W >> L;

    img = vector<vector<int>>(L, vector<int>(W));
    for (int y = 0; y < L; y++) {
        for (int x = 0; x < W; x++) {
            cin >> img[y][x];
        }
    }

    skeleton = Skeleton(img);
    vector<vector<int>> ridge_points = skeleton.getRidgePoints();
    vector<vector<int>> distance_map = skeleton.getDistanceMap();
    vector<data_tuple> tuples;
    for (int y = 0; y < L; y++) {
        for (int x = 0; x < W; x++) {
            if (ridge_points[y][x]) {
                data_tuple t = {x, y, distance_map[y][x]};
                tuples.push_back(t);
            }
        }
    }
    cout << tuples.size() << endl;
    for (data_tuple t : tuples) {
        cout << t.x << " " << t.y << " " << t.d << endl;
    }
    cout << endl;
}

/*
    Driver for taking in test cases and verifying the output.
*/
int main() {
    ios_base::sync_with_stdio(0); cin.tie(0);

    solveProblem();

    PNG recreated = skeleton.getRecreatedImage();

    // verifying the produced image
    for (int y = 0; y < img.size(); y++) {
        for (int x = 0; x < img[y].size(); x++) {
            if (!img[y][x] && recreated.getPixel(x, y) == WHITEPIXEL) {
                cout << "WRONG ANSWER: 1 at (" << x << "," << y << ") where should be 0" << endl;
                return 0;
            }
            if (img[y][x] && recreated.getPixel(x, y) == WHITEPIXEL &&
                skeleton.getDistanceMap()[y][x] != 1) {
                cout << "WRONG ANSWER: 0 at (" << x << "," << y << ") where should be 1" << endl;
            }
        }
    }
    cout << "CORRECT" << endl;
    return 0;
}

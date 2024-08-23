#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;

vector<Point> posList;
int width = 107, height = 48;

void savePositions() {
    ofstream outFile("CarParkPos", ios::binary);
    for (const auto& pos : posList) {
        outFile.write(reinterpret_cast<const char*>(&pos), sizeof(pos));
    }
    outFile.close();
}

void loadPositions() {
    ifstream inFile("CarParkPos", ios::binary);
    Point pos;
    while (inFile.read(reinterpret_cast<char*>(&pos), sizeof(pos))) {
        posList.push_back(pos);
    }
    inFile.close();
}

void mouseClick(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        posList.push_back(Point(x, y));
        savePositions();
    }
    else if (event == EVENT_RBUTTONDOWN) {
        for (auto it = posList.begin(); it != posList.end(); ++it) {
            if (x > it->x && x < it->x + width && y > it->y && y < it->y + height) {
                posList.erase(it);
                savePositions();
                break;
            }
        }
    }
}

int main() {
    // Load positions from file
    loadPositions();

    Mat img;
    while (true) {
        img = imread("Resources/carParkImg.png");
        for (const auto& pos : posList) {
            rectangle(img, pos, Point(pos.x + width, pos.y + height), Scalar(255, 0, 255), 2);
        }
        imshow("Image", img);
        setMouseCallback("Image", mouseClick);
        waitKey(1);
    }
    return 0;
}

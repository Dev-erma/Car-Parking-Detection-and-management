#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

vector<Point> posList;
int width = 107, height = 48;

void checkParkingSpace(Mat& img, Mat& imgProcessed) {
    int spaceCounter = 0;
    for (const auto& pos : posList) {
        Mat imgCrop = imgProcessed(Rect(pos.x, pos.y, width, height));
        int count = countNonZero(imgCrop);

        Scalar color;
        int thickness;
        if (count < 900) {
            color = Scalar(0, 255, 0);
            thickness = 5;
            spaceCounter++;
        }
        else {
            color = Scalar(0, 0, 255);
            thickness = 2;
        }
        rectangle(img, pos, Point(pos.x + width, pos.y + height), color, thickness);
        putText(img, to_string(count), Point(pos.x, pos.y + height - 3), FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
    }

    putText(img, "Free: " + to_string(spaceCounter) + "/" + to_string(posList.size()), Point(100, 50), FONT_HERSHEY_SIMPLEX, 2, Scalar(0, 200, 0), 5);
}

int main() {
    VideoCapture cap("Resources/carPark.mp4");

    ifstream inFile("CarParkPos", ios::binary);
    Point pos;
    while (inFile.read(reinterpret_cast<char*>(&pos), sizeof(pos))) {
        posList.push_back(pos);
    }
    inFile.close();

    Mat img, imgGray, imgBlur, imgThreshold, imgMedian, imgDilate;
    while (true) {
        if (cap.get(CAP_PROP_POS_FRAMES) == cap.get(CAP_PROP_FRAME_COUNT)) {
            cap.set(CAP_PROP_POS_FRAMES, 0);
        }
        bool success = cap.read(img);
        if (!success) break;

        cvtColor(img, imgGray, COLOR_BGR2GRAY);
        GaussianBlur(imgGray, imgBlur, Size(3, 3), 1);
        adaptiveThreshold(imgBlur, imgThreshold, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 25, 16);
        medianBlur(imgThreshold, imgMedian, 5);
        Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
        dilate(imgMedian, imgDilate, kernel);

        checkParkingSpace(img, imgDilate);
        imshow("Image", img);
        waitKey(1);
    }

    return 0;
}





























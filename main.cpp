#include <iostream>
#include <algorithm>
#include <functional>
#include <opencv2/opencv.hpp>

using namespace std;

uint32_t getZOrder(uint16_t xPos, uint16_t yPos) {
    static const uint32_t MASKS[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
    static const uint32_t SHIFTS[] = {1, 2, 4, 8};

    uint32_t x = xPos;
    uint32_t y = yPos;

    x = (x | (x << SHIFTS[3])) & MASKS[3];
    x = (x | (x << SHIFTS[2])) & MASKS[2];
    x = (x | (x << SHIFTS[1])) & MASKS[1];
    x = (x | (x << SHIFTS[0])) & MASKS[0];

    y = (y | (y << SHIFTS[3])) & MASKS[3];
    y = (y | (y << SHIFTS[2])) & MASKS[2];
    y = (y | (y << SHIFTS[1])) & MASKS[1];
    y = (y | (y << SHIFTS[0])) & MASKS[0];

    const uint32_t result = x | (y << 1);
    return result;
}

void fillImage(cv::InputOutputArray &img, const function<uint32_t(uint16_t, uint16_t)>& orderFunction) {
    using pos = struct{int x; int y; uint32_t h;};
    vector<pos> points;
    for (int i = 0; i < img.rows(); i+=20) {
        for (int j = 0; j < img.cols(); j+=20) {
            points.push_back({i, j, orderFunction(i, j)});
        }
    }
    sort(points.begin(), points.end(), [](pos a, pos b) {
        return a.h < b.h;
    });

    for (int i = 0; i < points.size() - 1; i++) {
        cv::line(img,{points[i].x, points[i].y},{points[i+1].x, points[i+1].y}, {0,0,0},2);
    }
}

int main() {
    cv::Mat img(1000,1000,CV_8UC3,cv::Scalar(255,255,255));

    fillImage(img, getZOrder);

    cv::imshow("Z order", img);
    cv::waitKey(0);

    return 0;
}

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

uint32_t getGrayOrder(uint16_t xPos, uint16_t yPos) {
    static vector<int> grayOrderSequence;
    if (grayOrderSequence.empty()) {
        for (int i =0; i<1<<12; i++) {
            grayOrderSequence.push_back(i^(i>>1));
        }
    }
    auto n = getZOrder(xPos, yPos);
    auto iter = find(grayOrderSequence.begin(), grayOrderSequence.end(), n);
    long index = iter - grayOrderSequence.begin();
    return index;
}

uint32_t getDoubleGrayOrder(uint16_t xPos, uint16_t yPos) {
    return getGrayOrder(xPos^(xPos>>1), yPos^(yPos>>1));
}

void fillImage(cv::InputOutputArray &img, const function<uint32_t(uint16_t, uint16_t)>& orderFunction) {
    using pos = struct{int x; int y; uint32_t h;};
    vector<pos> points;
    for (int i = 0; i < img.rows(); i+=20) {
        for (int j = 0; j < img.cols(); j+=20) {
            points.push_back({i, j, orderFunction(i/20, j/20)});
        }
    }
    sort(points.begin(), points.end(), [](pos a, pos b) {
        return a.h < b.h;
    });

    for (int i = 0; i < points.size() - 1; i++) {
        cv::Point p1 = {points[i].x, points[i].y};
        cv::Point p2 = {points[i+1].x, points[i+1].y};
        cv::line(img, p1, p2, {0,0,0},2);
    }
}

int main() {
    cv::Mat img(640,640,CV_8UC3,cv::Scalar(255,255,255));
    string windowName = "Space Filling Curves";
    cv::imshow(windowName, img);
    int option;
    do {
        cout << "Space Filling Curves" << endl;
        cout << "1. Z Order" << endl;
        cout << "2. Gray Order" << endl;
        cout << "3. Double Gray Order" << endl;
        cout << "Press q to quit the visualization" << endl;
        cout << "Enter 0 to exit the program" << endl;
        cout << "Enter number from 1 to 3: ";
        cin >> option;
        if (option == 0) break;
        img.setTo(cv::Scalar(255, 255, 255));
        switch (option) {
            case 1:
                fillImage(img, getZOrder);
                cv::imshow(windowName, img);
                break;
            case 2:
                fillImage(img, getGrayOrder);
                cv::imshow(windowName, img);
                break;
            case 3:
                fillImage(img, getDoubleGrayOrder);
                cv::imshow(windowName, img);
                break;
            default:
                fillImage(img, getZOrder);
                cv::imshow(windowName, img);
                break;
        }
        char c;
        do {
            c = (char)cv::waitKey(0);
        } while (c != 'q');
    } while (option != 0);

    return 0;
}

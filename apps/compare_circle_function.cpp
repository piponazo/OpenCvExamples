#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;

void fillCircle(Mat& image, int x0, int y0, int radius);
void drawScanLine2(Mat& image, int x0, int x1, int y);

int main()
{
    namedWindow("img1");
    namedWindow("img2");
    moveWindow("img1", 0, 0);
    moveWindow("img2", 0, 560);

    Mat img1 (480, 640, CV_8UC1, Scalar(0));
    Mat img2 (480, 640, CV_8UC1, Scalar(0));

    circle(img1, Point(30, 30), 20, Scalar(255), -1);
    fillCircle(img2, 30, 30, 20);

    imshow("img1", img1);
    imshow("img2", img2);
    waitKey(0);

    cout << "Application finished correctly\n";
    return EXIT_SUCCESS;
}

void fillCircle(Mat& image, int x0, int y0, int radius)
{
        int f = 1 - radius;
        int ddF_x = 1;
        int ddF_y = -2 * radius;
        int x = 0;
        int y = radius;

        drawScanLine2(image, x0 + radius, x0 - radius, y0);

        while (x < y)
        {
                // ddF_x == 2 * x + 1;
                // ddF_y == -2 * y;
                // f == x*x + y*y - radius*radius + 2*x - y + 1;
                if(f >= 0)
                {
                        y--;
                        ddF_y += 2;
                        f += ddF_y;
                }
                x++;
                ddF_x += 2;
                f += ddF_x;

                drawScanLine2(image, x0 + x, x0 - x, y0 + y);
                drawScanLine2(image, x0 + x, x0 - x, y0 - y);
                drawScanLine2(image, x0 + y, x0 - y, y0 + x);
                drawScanLine2(image, x0 + y, x0 - y, y0 - x);
        }
}

void drawScanLine2(Mat& image, int x0, int x1, int y)
{
        if (y < 0 || y >= image.rows)
        {
                return;
        }

        // sort from left to right and
        // clip scan line to image width
        int xa = std::max(std::min(x0, x1), 0);
        int xb = std::min(std::max(x0, x1), (int)image.cols);

        for (int x = xa; x < xb; x++)
        {
                image.at<uint8_t>(x, y) = 255;
        }
}

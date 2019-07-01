#include <opencv2/highgui/highgui.hpp>
#include <iostream>

static void onTrackbarParam1(int trackerPos, void* );

int param1=7;

int main()
{
    cv::Mat img(400, 400, CV_8UC1, cv::Scalar::all(255));

    // Common code
    cv::namedWindow("img", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    cv::moveWindow("img", 0, 0);

    // GTK code
//	cv::createTrackbar("param1", "img", &param1, 10, onTrackbarParam1);

    // Qt code
    cv::displayOverlay("img", "Press Ctrl+P for displaying the control panel", 1000);
    cv::createTrackbar("param1", std::string(), &param1, 10, onTrackbarParam1);
//    cv::setTrackbarPos("param1", std::string(), 5);

    cv::waitKey();

    return EXIT_SUCCESS;
}

static void onTrackbarParam1(int trackerPos , void* )
{
    std::cout << "trackerPos: " << trackerPos << std::endl;
}

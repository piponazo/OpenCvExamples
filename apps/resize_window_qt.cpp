#include <opencv2/highgui/highgui.hpp>

int main()
{
        cv::Mat img(400, 400, CV_8UC1, cv::Scalar::all(255));

        cv::namedWindow("img", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
        cv::moveWindow("img", 0, 0);
        cv::resizeWindow("img", 800, 800);

        cv::imshow("img", img);
        cv::waitKey();
}


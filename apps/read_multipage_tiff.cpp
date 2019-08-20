#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2) {
        cerr << "usage: appname image" << endl;
        return EXIT_FAILURE;
    }

    std::string filePath(argv[1]);
    std::vector<cv::Mat> pages;

    cv::imreadmulti	(filePath, pages, cv::IMREAD_UNCHANGED);
    cv::namedWindow ("img");

    double min, max;
    cv::Mat adjMap;
    for (size_t i=0; i<pages.size(); i++) {
        cv::minMaxIdx(pages[i], &min, &max);

        cout << "Image [" << i << "] size: " << pages[i].size() << " -Min: " << min << " -Max: " << max << endl;

        pages[i].convertTo(adjMap, CV_8UC1, 255 / (max-min), -255*min/(max-min));
        
        cv::imshow("img", adjMap);
        if (cv::waitKey() == 27) {
            break;
        }
    }

    //cout << "pages: " << pages.size() << endl;

    return EXIT_SUCCESS;
}

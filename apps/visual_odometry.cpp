// Some parts of the code has been inspired by https://github.com/avisingh599/mono-vo
#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video/tracking.hpp>

#include <filesystem>
#include <iostream>
#include <string>

using namespace std;
namespace fs = std::experimental::filesystem;

const int MIN_NUM_FEAT = 2000;

void featureDetection(const cv::Mat& img, vector<cv::Point2f>& points);
void featureTracking(const cv::Mat& img_1, const cv::Mat& img_2,
                     vector<cv::Point2f>& points1, vector<cv::Point2f>& points2,
                     vector<uchar>& status);

int main(int argc, char** argv)
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " path" << endl;
    }

    fs::path dataPath(argv[1]);
    auto imagesLeft = dataPath / "image_0";
    if (!fs::exists(dataPath) || !fs::is_directory(dataPath) || !fs::exists(imagesLeft)) {
        cerr << "Not valid data directory: " << dataPath << endl;
    }

    auto itImages = fs::directory_iterator(imagesLeft);

    cv::Mat imgPrevious = cv::imread(itImages->path().string());
    itImages++;
    cv::Mat imgCurrent = cv::imread(itImages->path().string());
    itImages++;
    std::vector<cv::Point2f> pointsPrev, pointsCurr;
    vector<uchar> status;

    featureDetection(imgPrevious, pointsPrev);
    featureTracking(imgPrevious, imgCurrent, pointsPrev, pointsCurr, status);

    cv::namedWindow("imgCurrent", cv::WINDOW_AUTOSIZE);


    for (const auto& entry : itImages) {
        imgPrevious = imgCurrent.clone();
        imgCurrent = cv::imread(entry.path().string());

        status.clear();
        featureTracking(imgPrevious, imgCurrent, pointsPrev, pointsCurr, status); //track those features to img_2

        cout << "Tracked features: " << pointsCurr.size() << endl;

        if (pointsPrev.size() < MIN_NUM_FEAT) {
            featureDetection(imgPrevious, pointsPrev);
            featureTracking(imgPrevious, imgCurrent, pointsPrev, pointsCurr, status);
        }

        pointsPrev = pointsCurr;

        cv::imshow("imgCurrent", imgCurrent);
        if (cv::waitKey(300) == 27)
            break;
    }

    return EXIT_SUCCESS;
}

void featureDetection(const cv::Mat& img, vector<cv::Point2f>& points)
{
    static vector<cv::KeyPoint> keypoints;
//    const int fast_threshold = 10;
//    const bool nonmaxSuppression = true;
//    cv::FAST(img, keypoints, fast_threshold, nonmaxSuppression);
    auto fast = cv::FastFeatureDetector::create();
    fast->detect(img, keypoints);
    cout << "N features: " << keypoints.size() << endl;
    cv::KeyPoint::convert(keypoints, points);

//    cv::Mat out;
//    cv::drawKeypoints(img, keypoints, out);
//    cv::imshow("imgFeatures", out);
}

void featureTracking(const cv::Mat& img_1, const cv::Mat& img_2,
                     vector<cv::Point2f>& points1, vector<cv::Point2f>& points2,
                     vector<uchar>& status)
{
    // this function automatically gets rid of points for which tracking fails
    vector<float> err;
    cv::Size winSize = cv::Size(21, 21);
    cv::TermCriteria termcrit = cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.01);

    cv::calcOpticalFlowPyrLK(img_1, img_2, points1, points2, status, err, winSize, 3, termcrit, 0, 0.001);

    // getting rid of points for which the KLT tracking failed or those who have gone outside the frame
    size_t indexCorrection = 0;
    for (size_t i = 0; i < status.size(); i++) {
        const cv::Point2f& pt = points2[i - indexCorrection];
        if ((status[i] == 0) || (pt.x < 0) || (pt.y < 0)) {
            if ((pt.x < 0) || (pt.y < 0)) {
                status[i] = 0;
            }
            points1.erase(points1.begin() + (i - indexCorrection));
            points2.erase(points2.begin() + (i - indexCorrection));
            indexCorrection++;
        }
    }

//    cv::Mat imgDraw = img_2.clone();
//    for (size_t i = 0; i < points1.size(); i++) {
//        if (status[i]) {
//            cv::line(imgDraw, points1[i], points2[i], cv::Scalar(255,255,255));
//        }
//    }
//    cv::imshow("imgTracking", imgDraw);
}

#define _USE_MATH_DEFINES

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

#include <cmath>

using namespace std;
using namespace cv;

static void detectFastKeypoints(Mat &img, vector<KeyPoint> &kps);
static void generateRandomKeypoints(const size_t n, const cv::Size size, vector<KeyPoint> &kps);
static void showDetectedKeypoints(const vector<KeyPoint> &kps, cv::Mat &imgShow);
static void supDiskCovering(const vector<KeyPoint> &kps, vector<KeyPoint> &outKps, const cv::Mat &imgShow);

static size_t desiredKeypoints = 50;

int main(int argc, char **argv)
{
    namedWindow("img");
    moveWindow("img", 0, 0);
    namedWindow("cells", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    moveWindow("cells", 0, 600);
    resizeWindow("cells", 500, 500);

    vector<KeyPoint> keypointsDetected, keypointsSelected;

    Mat img;
    if (argc == 2)
    {
        img = cv::imread(argv[1]);
        cv::imshow("img", img);
//        cv::resizeWindow("img", 600, 600);
        cv::waitKey(0);
        detectFastKeypoints(img, keypointsDetected);
    }
    else
    {
        img = Mat(500, 500, CV_8UC3, Scalar::all(0));
        generateRandomKeypoints(1000, img.size(), keypointsDetected);
    }
    showDetectedKeypoints(keypointsDetected, img);

    supDiskCovering(keypointsDetected, keypointsSelected, img);

    showDetectedKeypoints(keypointsSelected, img);

    cout << "Application finished correctly\n";
    return EXIT_SUCCESS;
}

static void detectFastKeypoints(Mat &img, vector<KeyPoint> &kps)
{
    auto fast = cv::FastFeatureDetector::create();
    fast->detect(img, kps);
    cout << "Keypoints detected: " << kps.size() << endl;
}

static void generateRandomKeypoints(const size_t n, const cv::Size size, vector<KeyPoint> &kps)
{
    kps.clear();
    kps.reserve(n);
    cv::RNG random;

    for(unsigned int i = 0; i < n; ++i) {
        kps.emplace_back(random.uniform(5, size.width - 5),
                         random.uniform(5, size.height - 5),
                         1.f,
                         -1.f,
                         random.uniform(0.f, 1.f),
                         0);
    }
    cout << "Keypoints generated: " << kps.size() << endl;
}

static void supDiskCovering(const vector<KeyPoint> &kps, vector<KeyPoint> &outKps, const cv::Mat &img)
{
    float e_r = 0.25f;
    float e_k = 0.25f;

    vector<KeyPoint> result;
    std::vector<KeyPoint> sorted_keypoints(kps);
    std::sort(sorted_keypoints.begin(), sorted_keypoints.end(), 
        [](const KeyPoint & kp1, const KeyPoint & kp2) -> bool { return kp1.response > kp2.response; });

    float prev_r_i = img.cols;
    float halfDist = prev_r_i / 2.f;
    cout << "Image size: " << img.size() << endl;

    while(result.size() < desiredKeypoints || result.size() > (1.f + e_k)*desiredKeypoints ) {
        float r_i;

        cv::Mat imgShow = img.clone();

        if (result.size() < desiredKeypoints) {
            r_i = prev_r_i - halfDist;
            halfDist = (prev_r_i - r_i) / 2.f;
        } else {
            r_i = prev_r_i + halfDist;
            halfDist = (r_i - prev_r_i) / 2.f;
        }
//        float cFloat = e_r * r_i / M_SQRT2;
        unsigned int c = std::floor(e_r * r_i / M_SQRT2 + 0.5f);

        result.clear();

        Mat imgCells (img.rows / c + 1, img.cols / c + 1, CV_8UC1, Scalar(0));
        cout << "prev_r_i: " << prev_r_i << endl;
        cout << "r_i: " << r_i << endl;
        cout << "c: " << c << endl;
        cout << "Size of imgCells: " << imgCells.size() << ", " << imgCells.total() << endl;

        // Draw cells
        for(int j = c; j < img.cols; j += c) {
            cv::line(imgShow, Point(j, 0), Point(j, img.rows - 1), Scalar(0, 255, 0));
        }
        for(int i = c; i < img.rows; i += c) {
            cv::line(imgShow, Point(0, i), Point(img.cols - 1, i), Scalar(0, 255, 0));
        }

        for (const KeyPoint & p_i: sorted_keypoints) {

            int cellX = p_i.pt.x / c;
            int cellY = p_i.pt.y / c;

            if (imgCells.at<uchar>(cellY, cellX) == 0) { // not covered

                result.push_back(p_i); // add current point to the result

                int cell_i_y = cellY * c + c/2;
                int cell_i_x = cellX * c + c/2;

                // cover all the cells with distance < r_i
                for(int i = 0; i < img.rows; i += c) {
                    for(int j = 0; j < img.cols; j += c) {
                        int cell_j_y = i + c/2;
                        int cell_j_x = j + c/2;
                        float dist = std::hypot(cell_i_x - cell_j_x, cell_i_y - cell_j_y);
                        if (dist < r_i) {
                            cv::rectangle(imgShow, Point(j, i), Point(j+c, i+c), Scalar(150, 0, 0), -1);
                        }
                    }
                }

                // Cover all the cells with distance smaller than r to the current cell center
//                for(int i = 0; i < imgCells.rows; i++) {
//                    for(int j = 0; j < imgCells.cols; j++) {
//                        int cell_j_y = i*c + c/2;
//                        int cell_j_x = j*c + c/2;
//                        float dist = std::hypot(cell_i_x - cell_j_x, cell_i_y - cell_j_y);
//                        if (dist < r_i) {
//                            imgCells.at<uchar>(i, j) = 255;
//                        }
//                    }
//                }

                // Cover all the cells using cv::circle
                int radiusCircle = static_cast<int>(r_i / c); 
                cv::circle(imgCells, Point(cellX, cellY), radiusCircle, Scalar(255), -1);

                cv::circle(imgShow, p_i.pt, 1, Scalar(255,0,255), 2, -1);
                cv::circle(imgShow, p_i.pt, r_i, Scalar(0,0,255), 1, 1);

                cv::imshow("img", imgShow);
                cv::imshow("cells", imgCells);
                if (cv::waitKey(0) == 27)
                    exit(EXIT_SUCCESS);

                size_t ones = cv::countNonZero(imgCells);

                if (imgCells.total() == ones) {
                    break;
                }
            }
        }

        prev_r_i = r_i;
        cout << "Output keypoints with current r: " << result.size() << endl << endl;
    }

    outKps.resize(desiredKeypoints);
    for (size_t i = 0; i < desiredKeypoints; i++) {
        outKps[i] = result[i];
    }
    
//    cv::imshow("img", img);
//    cv::waitKey(0);
}

static void showDetectedKeypoints(const vector<KeyPoint> &kps, cv::Mat &img)
{
    Mat imgShow = img.clone();
    int radius = std::floor(img.cols / 400 + 0.5f);

    for(const auto & kp : kps) {
        cv::circle(imgShow, cv::Point(kp.pt.x, kp.pt.y), radius, cv::Scalar::all(255 * kp.response), -1);
    }
    cv::imshow("img", imgShow);
    cv::waitKey(0);
}

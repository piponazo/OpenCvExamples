#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;

static void generateRandomKeypoints(const size_t n, const cv::Size size, vector<KeyPoint> &kps);
static void showDetectedKeypoints(const vector<KeyPoint> &kps, cv::Mat &imgShow);
static void supDiskCovering(const vector<KeyPoint> &kps, cv::Mat &imgShow);

static size_t desiredKeypoints = 50;

int main()
{
    namedWindow("img");
    namedWindow("cells", cv::WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
    moveWindow("img", 0, 0);
    moveWindow("cells", 0, 600);
    resizeWindow("cells", 500, 500);

    vector<KeyPoint> keypointsDetected, keypointsSelected;

    Mat img(500, 500, CV_8UC3, Scalar::all(0));

    generateRandomKeypoints(200, img.size(), keypointsDetected);
    supDiskCovering(keypointsDetected, img);
    showDetectedKeypoints(keypointsDetected, img);

    cv::imshow("img", img);
    cv::waitKey(0);

    cout << "Application finished correctly\n";
    return EXIT_SUCCESS;
}

static void supDiskCovering(const vector<KeyPoint> &kps, cv::Mat &img)
{
    float e_r = 0.25f;

    vector<KeyPoint> result;
    std::vector<KeyPoint> sorted_keypoints(kps);
    std::sort(sorted_keypoints.begin(), sorted_keypoints.end(), 
        [](const KeyPoint & kp1, const KeyPoint & kp2) -> bool { return kp1.response > kp2.response; });


    float prev_r_i = img.cols;
    float halfDist = prev_r_i / 2.f;
    while(result.size() != desiredKeypoints) {
        float r_i;

        img.setTo(Scalar::all(0));

        if (result.size() < desiredKeypoints) {
//            r_i = prev_r_i - prev_r_i / 2;
            r_i = prev_r_i - halfDist;
            halfDist = (prev_r_i - r_i) / 2.f;
        } else {
//            r_i = prev_r_i + prev_r_i / 2;
            r_i = prev_r_i + halfDist;
            halfDist = (r_i - prev_r_i) / 2.f;
        }
        unsigned int c = std::floor(e_r * r_i / M_SQRT2 + 0.5f);

        result.clear();

        Mat imgCells (img.rows / c, img.cols / c, CV_8UC1, Scalar(0));
        cout << "prev_r_i: " << prev_r_i << endl;
        cout << "r_i: " << r_i << endl;
        cout << "c: " << c << endl;
        cout << "Size of imgCells: " << imgCells.size() << ", " << imgCells.total() << endl;

        for(int j = c; j < img.cols; j += c) {
            cv::line(img, Point(j, 0), Point(j, img.rows - 1), Scalar(0, 255, 0));
        }
        for(int i = c; i < img.rows; i += c) {
            cv::line(img, Point(0, i), Point(img.cols - 1, i), Scalar(0, 255, 0));
        }


        size_t examinedKps = 0;
        for (const KeyPoint & p_i: sorted_keypoints) {

            int cellX = p_i.pt.x / c;
            int cellY = p_i.pt.y / c;
            examinedKps++;

    //        cout << "Value of cell : " << cellX << ", " << cellY << endl;
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
                            cv::rectangle(img, Point(j, i), Point(j+c, i+c), Scalar(150, 0, 0), -1);
                        }
                    }
                }

                for(int i = 0; i < imgCells.rows; i++) {
                    for(int j = 0; j < imgCells.rows; j++) {
                        int cell_j_y = i*c + c/2;
                        int cell_j_x = j*c + c/2;
                        float dist = std::hypot(cell_i_x - cell_j_x, cell_i_y - cell_j_y);
                        if (dist < r_i) {
                            imgCells.at<uchar>(i, j) = 255;
                        }
                    }
                }

                cv::circle(img, p_i.pt, 1, Scalar(255,0,255), 2, -1);
                cv::circle(img, p_i.pt, r_i, Scalar(0,0,255), 1, 1);

                cv::imshow("img", img);
                cv::imshow("cells", imgCells);

                size_t ones = cv::countNonZero(imgCells);
//                cout << "Ones detected: " << ones << endl;

                if (cv::waitKey(0) == 27)
                    exit(EXIT_SUCCESS);

                if (imgCells.total() == ones) {
                    break;
                }
            }
        }

        prev_r_i = r_i;
        cout << "Examined / total keypoints: " << examinedKps << " / " << sorted_keypoints.size() << endl;
        cout << "Output keypoints with current r: " << result.size() << endl << endl;
    }

    
//    cv::imshow("img", img);
//    cv::waitKey(0);
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
}

static void showDetectedKeypoints(const vector<KeyPoint> &kps, cv::Mat &img)
{
    for(const auto & kp : kps) {
        cv::circle(img, cv::Point(kp.pt.x, kp.pt.y), 2, cv::Scalar::all(255 * kp.response), -1);
    }
}

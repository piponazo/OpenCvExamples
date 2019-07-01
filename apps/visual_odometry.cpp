// Some parts of the code has been inspired by https://github.com/avisingh599/mono-vo
#include <opencv2/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video/tracking.hpp>

#include <filesystem>
#include <iterator>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
namespace fs = std::filesystem;

const int MINIMUM_TRACKED_FEATURES = 2000;

vector<cv::Point3d> readPoses(const string& path);
std::pair<double, cv::Point2d> readCameraInstrinsics(const string& path);

double getAbsoluteScale(const vector<cv::Point3d>& poses, int frame);
void featureDetection(const cv::Mat& img, vector<cv::Point2f>& points);
void featureTracking(const cv::Mat& img_1, const cv::Mat& img_2, vector<cv::Point2f>& points1,
                     vector<cv::Point2f>& points2, vector<uchar>& status);

int main(int argc, char** argv)
{
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " datasetPath sequence" << endl;
        return EXIT_FAILURE;
    }

    fs::path datasetPath(argv[1]);
    string sequence(argv[2]);

    fs::path dataPath = datasetPath / "sequences"/ sequence;
    fs::path imagesLeft = dataPath / "image_0";
    if (!fs::exists(dataPath) || !fs::is_directory(dataPath) || !fs::exists(imagesLeft)) {
        cerr << "Not valid data directory: " << dataPath << endl;
        return EXIT_FAILURE;
    }

    double focal;
    cv::Point2d pp;
    std::tie(focal, pp) = readCameraInstrinsics((dataPath / "calib.txt").string());

    sequence+=".txt";
    const auto poses = readPoses((datasetPath / "poses" / sequence).string());

    auto itImages = fs::directory_iterator(imagesLeft);

    cv::Mat imgPrevious = cv::imread(itImages->path().string());
    itImages++;
    cv::Mat imgCurrent = cv::imread(itImages->path().string());
    itImages++;
    std::vector<cv::Point2f> pointsPrev, pointsCurr;
    vector<uchar> status;

    featureDetection(imgPrevious, pointsPrev);
    featureTracking(imgPrevious, imgCurrent, pointsPrev, pointsCurr, status);

    // Compute Essential matrix and the pose
    cv::Mat E, R, t;
    E = cv::findEssentialMat(pointsCurr, pointsPrev, focal, pp, cv::RANSAC, 0.999, 1.0);
    cv::recoverPose(E, pointsCurr, pointsPrev, R, t, focal, pp);

    cv::Mat R_f = R.clone();
    cv::Mat t_f = t.clone();

//    cv::namedWindow("imgCurrent", cv::WINDOW_AUTOSIZE);
    int frame = 2;

    ofstream estimatedPoses("test.txt");

    for (const auto& entry : itImages) {
        imgPrevious = imgCurrent.clone();
        imgCurrent = cv::imread(entry.path().string());

        status.clear();
        featureTracking(imgPrevious, imgCurrent, pointsPrev, pointsCurr, status);  // track those features to img_2
        cout << "frame " << frame << " Tracked features: " << pointsCurr.size() << endl;

        E = cv::findEssentialMat(pointsCurr, pointsPrev, focal, pp, cv::RANSAC, 0.999, 1.0);
        cv::recoverPose(E, pointsCurr, pointsPrev, R, t, focal, pp);

        const double scale = getAbsoluteScale(poses, frame++);
        cout << "Scale: " << scale
             << "t: " << t.at<double>(0) << " , " << t.at<double>(1) << " , " << t.at<double>(2) << endl;

        if ((scale > 0.1) && (t.at<double>(2) > t.at<double>(0)) && (t.at<double>(2) > t.at<double>(1))) {
            t_f = t_f + scale * (R_f * t);
            R_f = R * R_f;
        }
        else {
             cout << "scale below 0.1, or incorrect translation" << endl;
        }

        double* rdata = R_f.ptr<double>(0);
        double* tdata = t_f.ptr<double>(0);
        estimatedPoses << rdata[0] << " " << rdata[1] << " " << rdata[2] << " " << tdata[0] << " "
                       << rdata[3] << " " << rdata[4] << " " << rdata[5] << " " << tdata[1]<< " "
                       << rdata[6] << " " << rdata[7] << " " << rdata[8] << " " << tdata[2] << endl;
//        estimatedPoses << t_f.at<double>(0) << ", " << t_f.at<double>(1) << ", " << t_f.at<double>(2) << endl;

        if (pointsPrev.size() < MINIMUM_TRACKED_FEATURES) {
            featureDetection(imgPrevious, pointsPrev);
            featureTracking(imgPrevious, imgCurrent, pointsPrev, pointsCurr, status);
        }

        pointsPrev = pointsCurr;

//        cv::imshow("imgCurrent", imgCurrent);
//        if (cv::waitKey(300) == 27)
//            break;
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

void featureTracking(const cv::Mat& img_1, const cv::Mat& img_2, vector<cv::Point2f>& points1,
                     vector<cv::Point2f>& points2, vector<uchar>& status)
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

vector<cv::Point3d> readPoses(const string& path)
{
    std::ifstream posesFile(path);
    if (!posesFile.is_open()) {
        cerr << "failed to open poses file: " << path << endl;
    }

    vector<cv::Point3d> points;
    points.reserve(1000);
    double val = 0., x = 0., y = 0., z = 0.;

    // Each line of the file has 12 fields. We read in the loop 11 and the last read allow us to check if there is a new
    // line
    posesFile >> val;
    while (!posesFile.eof()) {
        for (int i = 1; i < 12; i++) {  // Read here
            posesFile >> val;
            if (i == 3)
                x = val;
            else if (i == 7)
                y = val;
            else if (i == 11)
                z = val;
        }
        points.emplace_back(x, y, z);
        posesFile >> val;  // This is actually trying to read the next line
    }

    cout << "poses read: " << points.size() << endl;
    return points;
}

std::pair<double, cv::Point2d> readCameraInstrinsics(const string& path)
{
    ifstream calibFile(path);
    if (!calibFile.is_open()) {
        cerr << "failed to open calib file: " << path << endl;
    }
    string line;
    getline(calibFile, line);

    std::istringstream iss(line);
    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());

    double focal = std::stof(results[1]);
    cv::Point2d principalPoint (std::stof(results[3]), std::stof(results[7]));
    return make_pair(focal, principalPoint);
}

double getAbsoluteScale(const vector<cv::Point3d>& poses, int frame)
{
    auto& p1 = poses[frame - 1];
    auto& p2 = poses[frame];
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z));
}

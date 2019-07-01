#include "linearscale.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;

static void readPoints(const std::string &path, std::vector<cv::Point2f> &points);
static void shiftToMatSize(std::vector<cv::Point2f> &points, cv::Rect &rect);

static void locate_point(cv::Mat& img, cv::Subdiv2D& subdiv, cv::Point2f fp);
static void locateNearestPoints(cv::Mat& img, cv::Subdiv2D& subdiv, const cv::Point2f & fp);

static void draw_subdiv_point(cv::Mat& img, cv::Point2f fp, cv::Scalar color );
static void draw_subdiv( cv::Mat& img, cv::Subdiv2D& subdiv);

int main()
{
	std::vector<cv::Point2f> points;
	readPoints("../data/gps_positions_frienisber.csv", points);

	cv::Rect rect;
	shiftToMatSize(points, rect);

	
	cv::Subdiv2D subdiv(rect);
	cv::Mat img(rect.size(), CV_8UC3, cv::Scalar::all(0));
	
    cv::namedWindow("img", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
	cv::resizeWindow("img", 500, 500);
	cv::imshow("img", img);

	int key = 0, count = 0;
	for (const auto & point : points)
	{
        locate_point( img, subdiv, point);
//		if (count > 0)
//			locateNearestPoints( img, subdiv, point);

		cv::imshow("img", img );
		key = cv::waitKey(0);
		if (key == 27)
			break;
		
        subdiv.insert(point);

		locateNearestPoints( img, subdiv, point);
		cv::imshow( "img", img );
		key = cv::waitKey(0);
		if (key == 27)
			break;

        img = cv::Scalar::all(0);
        draw_subdiv( img, subdiv);
		cv::imshow( "img", img );
		key = cv::waitKey(0);
		if (key == 27)
			break;

		count++;
	}

    cout << "Application finished correctly\n";
    return EXIT_SUCCESS;
}

static void locate_point(cv::Mat& img,cv::Subdiv2D& subdiv, cv::Point2f fp)
{
	cv::Scalar active_facet_color(0, 0, 255);
    int edge0=0, vertex=0;

    subdiv.locate(fp, edge0, vertex);

    if( edge0 > 0 ) {
        int edge = edge0;
        do {
			cv::Point2f org, dst;
            if( subdiv.edgeOrg(edge, &org) > 0 && subdiv.edgeDst(edge, &dst) > 0 ) {
                line( img, org, dst, active_facet_color, 3, cv::LINE_AA, 0 );
			}

            edge = subdiv.getEdge(edge, cv::Subdiv2D::NEXT_AROUND_LEFT);
        } while( edge != edge0 );
    }

    draw_subdiv_point( img, fp, active_facet_color );
}

static void locateNearestPoints(cv::Mat& img, cv::Subdiv2D& subdiv, const cv::Point2f & fp)
{
    int edge0=0, vertex=0;
    subdiv.locate(fp, edge0, vertex);

//	subdiv.getEdge(edge0, PREV_AROUND_ORG);

//	cv::Point2f pOrg, pDst;
//	subdiv.edgeOrg(edge0, &pOrg);
//	subdiv.edgeDst(edge0, &pDst);
//    draw_subdiv_point( img, pOrg, cv::Scalar(255, 0, 0) );
//    draw_subdiv_point( img, pDst, cv::Scalar(255, 255, 0) );

	cv::Point2f nearest;
	subdiv.findNearest(fp, &nearest);
	cout << "Curent  point: " << fp << endl;
	cout << "Nearest point: " << nearest << endl;
	cv::Scalar pointColor(255, 0, 0);
    draw_subdiv_point( img, nearest, pointColor ); // right now is the previous point
}

static void draw_subdiv_point(cv::Mat& img, cv::Point2f fp, cv::Scalar color )
{
    cv::circle( img, fp, 3, color, cv::FILLED, 8, 0 );
}

static void draw_subdiv( cv::Mat& img, cv::Subdiv2D& subdiv)
{
	cv::Scalar delaunay_color(255,255,255);
//	std::vector<cv::Vec6f> triangleList;
//    subdiv.getTriangleList(triangleList);
//	std::vector<cv::Point> pt(3);
//
//    for( size_t i = 0; i < triangleList.size(); i++ )
//    {
//		cv::Vec6f t = triangleList[i];
//        pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
//        pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
//        pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));
//        line(img, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
//        line(img, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
//        line(img, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);
//    }

	std::vector<cv::Vec4f> edgeList;
    subdiv.getEdgeList(edgeList);
    for( size_t i = 0; i < edgeList.size(); i++ )
    {
		cv::Vec4f e = edgeList[i];
		cv::Point pt0 = cv::Point(cvRound(e[0]), cvRound(e[1]));
		cv::Point pt1 = cv::Point(cvRound(e[2]), cvRound(e[3]));
        cv::line(img, pt0, pt1, delaunay_color, 1, cv::LINE_AA, 0);
    }
}

static void readPoints(const std::string &path, std::vector<cv::Point2f> &points)
{
    std::ifstream csvFile(path.c_str());
    if(csvFile.is_open() == false) {
        cerr << "Error reading the input CSV file\n";
        exit(EXIT_FAILURE);
    }

    std::string line;
    std::getline(csvFile, line); // read the header of the file
    while(csvFile.good()) {
		std::getline(csvFile, line, ',');
		float lat = std::stof(line);
		std::getline(csvFile, line, ',');
		float lng = std::stof(line);
		std::getline(csvFile, line);
		points.emplace_back(lng, lat);
		csvFile.peek();
    }
	csvFile.close();
    cout << "Points read from the CSV file: " << points.size() << endl;
}

static void shiftToMatSize(std::vector<cv::Point2f> &points, cv::Rect &rect)
{
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();
	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float meanY = 0.f, meanX = 0.f;

	// Calculate min/max of Lat (y) and Lng (x)
	for(const auto &point: points) {
		minY = std::min(minY, point.y);
		maxY = std::max(maxY, point.y);
		minX = std::min(minX, point.x);
		maxX = std::max(maxX, point.x);
		meanY += point.y;
		meanX += point.x;
	}
	meanY /= points.size();
	meanX /= points.size();

	// Calculate stdDev of Lat and Lng
	double yStdDev = 0., xStdDev = 0.;
	for(const auto &point: points) {
			yStdDev += (point.y - meanY) * (point.y - meanY);
			xStdDev += (point.x - meanX) * (point.x - meanX);
	}
	yStdDev = std::sqrt(yStdDev / points.size());
	xStdDev = std::sqrt(xStdDev / points.size());

	// In that way we can create a rect depicting the real distribution of the GPS positions
	int rows = static_cast<int>((maxY - minY) / yStdDev * 100.);
    int cols = static_cast<int>((maxX - minX) / xStdDev * 100.);

	rect = cv::Rect(0,0, cols, rows);
	cout << "rect: " << rect << endl;

	LinearScale scalerY(minY, maxY, rows-5, 5); // invert the direction since in OpenCV positive Y points down
	LinearScale scalerX(minX, maxX, 5, cols-5);

	// Let's scale the values
	for(size_t i = 0; i < points.size(); ++i) {
		auto &point = points[i];
		point = cv::Point2f(scalerX(point.x), scalerY(point.y));
	}
}

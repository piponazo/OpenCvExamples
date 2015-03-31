#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;

//static void readPoints(const std::string &path, std::vector<cv::Point2f> &points);
static void simulateFlightPlan(std::vector<cv::Point2f> &points);
static void locate_point(cv::Mat& img,cv::Subdiv2D& subdiv, cv::Point2f fp, cv::Scalar active_color );
static void draw_subdiv_point(cv::Mat& img, cv::Point2f fp, cv::Scalar color );
static void draw_subdiv( cv::Mat& img, cv::Subdiv2D& subdiv, cv::Scalar delaunay_color );

int main()
{
	std::vector<cv::Point2f> points;
//	readPoints("../data/gps_positions_frienisber.csv", points);
	simulateFlightPlan( points);

	cv::Scalar active_facet_color(0, 0, 255), delaunay_color(255,255,255);
	cv::Rect rect(0, 0, 600, 600);
	
	cv::Subdiv2D subdiv(rect);
	cv::Mat img(rect.size(), CV_8UC3, cv::Scalar::all(0));
	
	cv::namedWindow("img");
	cv::imshow("img", img);

	for (const auto & p : points)
	{
        locate_point( img, subdiv, p, active_facet_color );
		cv::imshow("img", img );
		cv::waitKey(0);
		
        subdiv.insert(p);

        img = cv::Scalar::all(0);
        draw_subdiv( img, subdiv, delaunay_color );
		cv::imshow( "img", img );
		cv::waitKey(0);
	}

    cout << "Application finished correctly\n";
    return EXIT_SUCCESS;
}

//static void readPoints(const std::string &path, std::vector<cv::Point2f> &points)
//{
//    std::ifstream csvFile(path.c_str());
//    if(csvFile.is_open() == false) {
//        cerr << "Error reading the input CSV file\n";
//        exit(EXIT_FAILURE);
//    }
//
//    std::string line;
//    std::getline(csvFile, line); // read the header of the file
//    while(csvFile.good()) {
//		std::getline(csvFile, line, ',');
//		float lat = std::stof(line);
//		std::getline(csvFile, line, ',');
//		float lng = std::stof(line);
//		std::getline(csvFile, line);
//		points.emplace_back(lng, lat);
//		csvFile.peek();
//    }
//	csvFile.close();
//    cout << "Points read from the CSV file: " << points.size() << endl;
//}

static void simulateFlightPlan(std::vector<cv::Point2f> &points)
{
	for (int i=5; i<595; i+=50) {
		for (int j=5; j<595; j+=80) {
			points.emplace_back(i,j);
		}
	}
}

static void locate_point(cv::Mat& img,cv::Subdiv2D& subdiv, cv::Point2f fp, cv::Scalar active_color )
{
    int e0=0, vertex=0;

    subdiv.locate(fp, e0, vertex);

    if( e0 > 0 )
    {
        int e = e0;
        do
        {
			cv::Point2f org, dst;
            if( subdiv.edgeOrg(e, &org) > 0 && subdiv.edgeDst(e, &dst) > 0 )
                line( img, org, dst, active_color, 3, CV_AA, 0 );

            e = subdiv.getEdge(e, cv::Subdiv2D::NEXT_AROUND_LEFT);
        }
        while( e != e0 );
    }

    draw_subdiv_point( img, fp, active_color );
}

static void draw_subdiv_point(cv::Mat& img, cv::Point2f fp, cv::Scalar color )
{
	cv::circle( img, fp, 3, color, CV_FILLED, 8, 0 );
}

static void draw_subdiv( cv::Mat& img, cv::Subdiv2D& subdiv, cv::Scalar delaunay_color )
{
	std::vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
	std::vector<cv::Point> pt(3);

    for( size_t i = 0; i < triangleList.size(); i++ )
    {
		cv::Vec6f t = triangleList[i];
        pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
        pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));
        line(img, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
        line(img, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
        line(img, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);
    }
//    vector<Vec4f> edgeList;
//    subdiv.getEdgeList(edgeList);
//    for( size_t i = 0; i < edgeList.size(); i++ )
//    {
//        Vec4f e = edgeList[i];
//        Point pt0 = Point(cvRound(e[0]), cvRound(e[1]));
//        Point pt1 = Point(cvRound(e[2]), cvRound(e[3]));
//        line(img, pt0, pt1, delaunay_color, 1, CV_AA, 0);
//    }
}


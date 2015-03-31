#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;

int main()
{
    std::ifstream csvFile("../data/gps_positions_frienisber.csv");
    if(csvFile.is_open() == false) {
        cerr << "Error reading the input CSV file\n";
        return EXIT_FAILURE;
    }

    std::string line;
    std::getline(csvFile, line); // read the header of the file
	std::vector<cv::Point2f> points;
    while(csvFile.good()) {
		std::getline(csvFile, line, ',');
		cout << "word: " << line << endl;
		float lat = std::stof(line);
		std::getline(csvFile, line, ',');
		cout << "word: " << line << endl;
		float lng = std::stof(line);
		std::getline(csvFile, line);
		points.emplace_back(lng, lat);
		csvFile.peek();
    }
	csvFile.close();
    cout << "Points read from the CSV file: " << points.size() << endl;

	cv::Scalar active_facet_color(0, 0, 255), delaunay_color(255,255,255);
	cv::Rect rect(0, 0, 600, 600);
	
	cv::Subdiv2D subdiv(rect);
	cv::Mat img(rect.size(), CV_8UC3, cv::Scalar::all(0));
	
	cv::namedWindow("img");
	cv::imshow("img", img);

    cout << "Application finished correctly\n";
    return EXIT_SUCCESS;
}

/*this creates a yaml or xml list of files from the command line args
 */

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <string>

using namespace std;
using namespace cv;

const String keys =
    "{help h usage ? |              | print this message  }"
    "{@output        | images.yml   | Output file where to write the list of images. }";

static void help(char** av)
{
    cout << "\nCreates a yaml or xml list of files from the command line args\n"
            "usage:  ./"
         << av[0] << " -o imagelist.yaml images/*.jpg\n"
         << "This will serialize the list of images with opencv's FileStorage framework" << endl;
}

int main(int ac, char** av)
{
    cv::CommandLineParser parser(ac, av, keys);
    if (parser.has("help")) {
        parser.printMessage();
        help(av);
        return 0;
    }

    string outputname = parser.get<string>("@output");

    if (outputname.empty()) {
        parser.printMessage();
        help(av);
        return 1;
    }

    FileStorage fs(outputname, FileStorage::WRITE);
    fs << "images"
       << "[";
    for (int i = 2; i < ac; i++) {
        fs << string(av[i]);
    }
    fs << "]";
    return 0;
}

#include <opencv2/core/core.hpp>

#include <string>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " path" << endl;
    }

    fs::path dataPath(argv[1]);
    auto imagesLeft = dataPath / "image_0";
    if (!fs::exists(dataPath) || !fs::is_directory(dataPath) || !fs::exists(imagesLeft))
    {
        cerr << "Not valid data directory: " << dataPath << endl;
    }

    for (const auto& entry : fs::directory_iterator(imagesLeft))
    {
        auto filename = entry.path().filename();
        cout << "File name: " << filename << endl;
    }

    return EXIT_SUCCESS;
}

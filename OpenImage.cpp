
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;


class ReadImage {
public:
    ReadImage(const string& imagePath) : imagePath(imagePath) {}

    void displayImage() {
        Mat img = imread(imagePath);
        if (img.empty()) {
            cerr << "Error: Could not open or find the image." << endl;
            return;
        }

        namedWindow("First OpenCV Application", WINDOW_AUTOSIZE);
        imshow("First OpenCV Application", img);
        moveWindow("First OpenCV Application", 0, 45);
        waitKey(0); // Wait for a keystroke in the window
        destroyAllWindows();
    }

private:
    string imagePath;
};

/*
  int main() {
    string imagePath = "C:/Users/Keks/Desktop/IMG_0590.jpg";
    ReadImage reader(imagePath);
    reader.displayImage();
    return 0;
}
*/


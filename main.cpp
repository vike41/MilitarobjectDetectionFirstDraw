#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>  
#include <opencv2/objdetect.hpp> 

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char** argv){

    // Get file location
    string video_path = "D:\\Video\\OpenCV_Refs\\colona_ref.mp4";
    cout << "Attempting to open file: " << video_path << endl;

    // Check that video is opened
    VideoCapture video_file(video_path);
    if (!video_file.isOpened()) {
        cerr << "Error when reading file" << endl;
        return -1;
    }

    //Load car cascade classifier
    CascadeClassifier car_cascade;
    if (!car_cascade.load("harrcascade_car.xml")) {
        cerr << "Error loading car cascade file " << endl;
        return -1;
    }

    // Get Video Resolution
    int frameWidth = video_file.get(CAP_PROP_FRAME_WIDTH);
    int frameHeight = video_file.get(CAP_PROP_FRAME_HEIGHT);

    cout << "File opened successfully" << endl;

    Mat frame, gray_frame, blurred_frame, dilated_frame, morph_frame;
    namedWindow("w", 1);

    for (;;)
    {
        video_file >> frame;
        if (frame.empty()) {
            cout << "End of video or error reading frame" << endl;
            break;
        }

        // Convert frame to grayscale
        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);

        // Apply Gaussian blur to the grayscale frame
        GaussianBlur(gray_frame, blurred_frame, Size(15, 15), 1);

        // Apply Dilate
        Mat element_dilate = getStructuringElement(MORPH_RECT, Size(5, 5));
        dilate(blurred_frame, dilated_frame, element_dilate);

        //Apply morphological closing 
        morphologyEx(dilated_frame, morph_frame, MORPH_CLOSE, element_dilate);

        // Detect cars
        vector<Rect> cars;
        car_cascade.detectMultiScale(morph_frame, cars, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        // Draw rectangles around detected cars
        for (size_t i = 0; i < cars.size(); i++) {
            rectangle(frame, cars[i], Scalar(255, 0, 0), 2, 8, 0);
        }

        // Display the frame with detected cars
        imshow("w", frame);
        if (waitKey(20) >= 0) break; // waits to display frame and breaks if a key is pressed
    }

    return 0;
    }
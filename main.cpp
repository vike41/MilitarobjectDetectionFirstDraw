// Autor: Vitalii Kens
// LinkedIn: 

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>  
#include <opencv2/tracking.hpp>  // Include the tracking module

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

// Global variables to store the coordinates of the ROI
Rect2d roi;
bool isSelectingROI = false;
bool roiSelected = false;

// Mouse callback function
void mouseHandler(int event, int x, int y, int, void*) {
    static Point topLeft;
    if (event == EVENT_LBUTTONDOWN) {
        if (!isSelectingROI) {
            topLeft = Point(x, y);
            isSelectingROI = true;
            roiSelected = false;
        }
        else {
            Point bottomRight = Point(x, y);
            roi = Rect2d(topLeft, bottomRight);
            isSelectingROI = false;
            roiSelected = true;
        }
    }
}

int main(int argc, char** argv) {

    // Get file location
    string video_path = "D:\\Video\\OpenCV_Refs\\colona_ref.mp4";
    cout << "Attempting to open file: " << video_path << endl;

    // Check that video is opened
    VideoCapture video_file(video_path);
    if (!video_file.isOpened()) {
        cerr << "Error when reading file" << endl;
        return -1;
    }

    // Get Video Resolution
    int frameWidth = video_file.get(CAP_PROP_FRAME_WIDTH);
    int frameHeight = video_file.get(CAP_PROP_FRAME_HEIGHT);

    cout << "File opened successfully" << endl;

    Mat frame;
    namedWindow("Video", 1);
    setMouseCallback("Video", mouseHandler);

    Ptr<Tracker> tracker;
    bool isTracking = false;

    for (;;) {
        video_file >> frame;
        if (frame.empty()) {
            cout << "End of video or error reading frame" << endl;
            break;
        }

        // Select ROI
        if (roiSelected && !isTracking) {
            tracker = TrackerCSRT::create();
            tracker->init(frame, roi);
            isTracking = true;
        }

        // Update the tracking result
        if (isTracking) {
            bool ok = tracker->update(frame, roi);
            if (ok) {
                // Tracking success: Draw the tracked object
                rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);
            }
            else {
                // Tracking failure: Show message
                putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
                isTracking = false;
            }
        }

        // Draw the ROI selection rectangle
        if (isSelectingROI) {
            rectangle(frame, roi, Scalar(0, 255, 0), 2);
        }

        // Display the frame
        imshow("Video", frame);
        if (waitKey(20) >= 0) break; // waits to display frame and breaks if a key is pressed
    }

    return 0;
}

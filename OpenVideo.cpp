
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp> 

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

class VideoPlayer {

public:
    VideoPlayer(const string& videoPath) : videoPath(videoPath) {}

    bool openVideo() {
        cout << "Attempting to open file: " << videoPath << endl;
        videoCapture.open(videoPath);
        if (!videoCapture.isOpened()) {
            cerr << "Error when reading file" << endl;
            return false;
        }
        cout << "File opened successfully" << endl;

        frameWidth = videoCapture.get(CAP_PROP_FRAME_WIDTH);
        frameHeight = videoCapture.get(CAP_PROP_FRAME_HEIGHT);

        return true;
    }

    void playVideo() {
        if (!videoCapture.isOpened()) {
            cerr << "Video file is not opened. Cannot play video." << endl;
            return;
        }

        namedWindow("Video Window", 1);

        Mat frame;
        for (;;) {
            videoCapture >> frame;
            if (frame.empty()) {
                cout << "End of video or error reading frame" << endl;
                break;
            }
            imshow("Video Window", frame);
            if (waitKey(20) >= 0) break; // waits to display frame and breaks if a key is pressed
        }

        waitKey(0); // key press to close window
        destroyAllWindows();
    }

private:
    string videoPath;
    VideoCapture videoCapture;
    int frameWidth;
    int frameHeight;
};

/*
int main(int argc, char** argv) {
    string videoPath = "D:\\Video\\OpenCV_Refs\\colona_ref.mp4";
    VideoPlayer player(videoPath);

    if (!player.openVideo()) {
        return -1;
    }

    player.playVideo();
    return 0;
}
*/

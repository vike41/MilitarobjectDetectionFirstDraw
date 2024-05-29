#include <opencv2/opencv.hpp>
#include <iostream>

class VideoProcessor {
private:
    cv::VideoCapture cap;
    cv::Mat frame;
    std::vector<cv::Mat> bgr_planes;
    int histSize;
    float range[2];
    const float* histRange;
    cv::Mat histImage;

public:
    VideoProcessor(const std::string& filename, int histSize = 256)
        : histSize(histSize), histRange(range) {
        // Attempt to open the video file
        cap.open(filename);

        // Check if the video file opened successfully
        if (!cap.isOpened()) {
            std::cerr << "Error: Unable to open video file." << std::endl;
            return;
        }

        // Define histogram range
        range[0] = 0;
        range[1] = 256;
    }

    void processVideo() {
        while (true) {
            // Read a frame from the video
            cap >> frame;

            // If the frame is empty, break the loop
            if (frame.empty()) break;

            // Split the frame into three channels: B, G, and R
            cv::split(frame, bgr_planes);

            // Compute and draw histograms
            computeAndDrawHistograms();

            // Display the frame and histogram
            cv::imshow("Frame", frame);
            cv::imshow("Histogram", histImage);

            // Wait for a key press, break the loop if 'q' is pressed
            if (cv::waitKey(25) == 'q') break;
        }
    }

private:
    void computeAndDrawHistograms() {
        // Compute the histograms
        cv::Mat b_hist, g_hist, r_hist;
        cv::calcHist(&bgr_planes[0], 1, nullptr, cv::Mat(), b_hist, 1, &histSize, &histRange, true, false);
        cv::calcHist(&bgr_planes[1], 1, nullptr, cv::Mat(), g_hist, 1, &histSize, &histRange, true, false);
        cv::calcHist(&bgr_planes[2], 1, nullptr, cv::Mat(), r_hist, 1, &histSize, &histRange, true, false);

        // Normalize the histograms
        cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
        cv::normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
        cv::normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

        // Draw the histograms
        int hist_w = 512, hist_h = 400;
        int bin_w = cvRound((double)hist_w / histSize);
        histImage = cv::Mat(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

        for (int i = 1; i < histSize; i++) {
            cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
                cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
                cv::Scalar(255, 0, 0), 2, 8, 0);
            cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
                cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
                cv::Scalar(0, 255, 0), 2, 8, 0);
            cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
                cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
                cv::Scalar(0, 0, 255), 2, 8, 0);
        }
    }
};
/*
int main() {
    VideoProcessor videoProc("D:\\Video\\OpenCV_Refs\\colona_ref.mp4");
    videoProc.processVideo();
    return 0;
}
*/

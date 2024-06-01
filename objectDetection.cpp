// firt draw tracker. 
#include "centroid_tracker.h"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iterator>
#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <set>
#include <algorithm>


class CentroidTracker {
public:
    CentroidTracker(int maxDisappeared);

    std::vector<std::pair<int, std::pair<int, int>>> update(const std::vector<std::vector<int>>& boundingBoxes);

    std::map<int, std::vector<std::pair<int, int>>> pathKeeper;

private:
    int nextObjectID;
    int maxDisappeared;
    std::vector<std::pair<int, std::pair<int, int>>> trackedObjects;
    std::map<int, int> disappearedObjects;

    double calculateDistance(double x1, double y1, double x2, double y2);
    void registerObject(int cX, int cY);
};

CentroidTracker::CentroidTracker(int maxDisappeared)
    : nextObjectID(0), maxDisappeared(maxDisappeared) {}

double CentroidTracker::calculateDistance(double x1, double y1, double x2, double y2) {
    double deltaX = x1 - x2;
    double deltaY = y1 - y2;
    return std::sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

void CentroidTracker::registerObject(int cX, int cY) {
    trackedObjects.push_back({ nextObjectID, {cX, cY} });
    disappearedObjects[nextObjectID] = 0;
    nextObjectID++;
}

std::vector<std::pair<int, std::pair<int, int>>> CentroidTracker::update(const std::vector<std::vector<int>>& boundingBoxes) {
    if (boundingBoxes.empty()) {
        for (auto it = disappearedObjects.begin(); it != disappearedObjects.end(); ) {
            it->second++;
            if (it->second > maxDisappeared) {
                auto objectID = it->first;
                trackedObjects.erase(std::remove_if(trackedObjects.begin(), trackedObjects.end(), [objectID](auto& obj) {
                    return obj.first == objectID;
                    }), trackedObjects.end());

                pathKeeper.erase(objectID);
                it = disappearedObjects.erase(it);
            }
            else {
                ++it;
            }
        }
        return trackedObjects;
    }

    std::vector<std::pair<int, int>> inputCentroids;
    for (const auto& box : boundingBoxes) {
        int cX = (box[0] + box[2]) / 2;
        int cY = (box[1] + box[3]) / 2;
        inputCentroids.push_back({ cX, cY });
    }

    if (trackedObjects.empty()) {
        for (const auto& centroid : inputCentroids) {
            registerObject(centroid.first, centroid.second);
        }
    }
    else {
        std::vector<int> objectIDs;
        std::vector<std::pair<int, int>> objectCentroids;
        for (const auto& object : trackedObjects) {
            objectIDs.push_back(object.first);
            objectCentroids.push_back(object.second);
        }

        std::vector<std::vector<float>> distances;
        for (const auto& objCentroid : objectCentroids) {
            std::vector<float> distRow;
            for (const auto& inpCentroid : inputCentroids) {
                distRow.push_back(calculateDistance(objCentroid.first, objCentroid.second, inpCentroid.first, inpCentroid.second));
            }
            distances.push_back(distRow);
        }

        std::vector<int> cols;
        for (const auto& distRow : distances) {
            auto minElem = std::min_element(distRow.begin(), distRow.end());
            cols.push_back(std::distance(distRow.begin(), minElem));
        }

        std::vector<std::pair<float, int>> sortedRows;
        for (int i = 0; i < distances.size(); ++i) {
            sortedRows.push_back({ distances[i][cols[i]], i });
        }
        std::sort(sortedRows.begin(), sortedRows.end());

        std::set<int> usedRows, usedCols;
        for (const auto& [dist, row] : sortedRows) {
            if (usedRows.count(row) || usedCols.count(cols[row])) continue;
            int objectID = objectIDs[row];
            trackedObjects[row].second = inputCentroids[cols[row]];
            disappearedObjects[objectID] = 0;
            usedRows.insert(row);
            usedCols.insert(cols[row]);
        }

        std::set<int> allRows, allCols;
        for (int i = 0; i < objectCentroids.size(); ++i) allRows.insert(i);
        for (int i = 0; i < inputCentroids.size(); ++i) allCols.insert(i);

        std::set<int> unusedRows, unusedCols;
        std::set_difference(allRows.begin(), allRows.end(), usedRows.begin(), usedRows.end(), std::inserter(unusedRows, unusedRows.begin()));
        std::set_difference(allCols.begin(), allCols.end(), usedCols.begin(), usedCols.end(), std::inserter(unusedCols, unusedCols.begin()));

        if (objectCentroids.size() >= inputCentroids.size()) {
            for (const auto& row : unusedRows) {
                int objectID = objectIDs[row];
                disappearedObjects[objectID]++;
                if (disappearedObjects[objectID] > maxDisappeared) {
                    trackedObjects.erase(std::remove_if(trackedObjects.begin(), trackedObjects.end(), [objectID](auto& obj) {
                        return obj.first == objectID;
                        }), trackedObjects.end());
                    pathKeeper.erase(objectID);
                    disappearedObjects.erase(objectID);
                }
            }
        }
        else {
            for (const auto& col : unusedCols) {
                registerObject(inputCentroids[col].first, inputCentroids[col].second);
            }
        }
    }

    for (const auto& obj : trackedObjects) {
        auto& path = pathKeeper[obj.first];
        if (path.size() > 30) path.erase(path.begin());
        path.push_back(obj.second);
    }

    return trackedObjects;
}
/*
int main() {
    std::cout << "Initializing Centroid Tracker..." << std::endl;
    CentroidTracker tracker(20);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open camera." << std::endl;
        return -1;
    }

    std::string modelTxt = "../model/deploy.prototxt";
    std::string modelBin = "../model/res10_300x300_ssd_iter_140000.caffemodel";

    std::cout << "Loading model..." << std::endl;
    cv::dnn::Net net = cv::dnn::readNetFromCaffe(modelTxt, modelBin);

    std::cout << "Starting video stream..." << std::endl;
    while (cap.isOpened()) {
        cv::Mat frame;
        cap.read(frame);

        cv::resize(frame, frame, cv::Size(400, 300));
        cv::Mat inputBlob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(400, 300), cv::Scalar(104.0, 177.0, 123.0));

        net.setInput(inputBlob);
        cv::Mat detection = net.forward("detection_out");
        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

        std::vector<std::vector<int>> boundingBoxes;
        float confidenceThreshold = 0.2;
        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);
            if (confidence > confidenceThreshold) {
                int xLeftTop = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                int yLeftTop = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                int xRightBottom = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                int yRightBottom = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                boundingBoxes.push_back({ xLeftTop, yLeftTop, xRightBottom, yRightBottom });
                cv::rectangle(frame, cv::Rect(xLeftTop, yLeftTop, xRightBottom - xLeftTop, yRightBottom - yLeftTop), cv::Scalar(0, 255, 0), 2);
            }
        }

        auto trackedObjects = tracker.update(boundingBoxes);
        for (const auto& obj : trackedObjects) {
            cv::circle(frame, cv::Point(obj.second.first, obj.second.second), 4, cv::Scalar(255, 0, 0), -1);
            cv::putText(frame, std::to_string(obj.first), cv::Point(obj.second.first - 10, obj.second.second - 10), cv::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
        }


*/


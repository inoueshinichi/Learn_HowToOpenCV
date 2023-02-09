/**
 * @file harris_corner_detector.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

// スルー画像の取り込み処理
CV_DEFINE_PROCESSFRAME

int main(int argc, char **argv)
{
    try
    {
        // Harris
        std::vector<cv::Point2f> corners;
        const int maxCorners = 1000; // 検出する角の上限
        const double qualityLevel = 0.1;
        const double minDistance = 5;
        const int blockSize = 3;

        auto local_features = [&](const cv::Mat &curFrame, const cv::Mat &lastFrame) -> void
        {
            cv::Mat img_gray;
            cv::cvtColor(curFrame, img_gray, cv::COLOR_BGR2GRAY);

            // cv::goodFeaturesToTrack(img_gray, corners, maxCorners, qualityLevel, minDistance,
            //                         cv::Mat(), blockSize, true); // 第8引数をtrueにすることでHarrisCornerを有効化
            cv::Ptr<cv::Feature2D> detector = cv::GFTTDetector::create(maxCorners,
                                                                       qualityLevel,
                                                                       minDistance,
                                                                       blockSize,
                                                                       true);

            std::vector<cv::KeyPoint> keyPoints;

            detector->detect(img_gray, keyPoints);

            for (int i = 0; i < keyPoints.size(); i++)
            {
                cv::circle(curFrame,
                           cv::Point(keyPoints[i].pt.x, keyPoints[i].pt.y),
                           3,
                           cv::Scalar(0, 0, 255),
                           2);
            }

            CV_IMSHOW(curFrame)
            // CV_IMSHOW(lastFrame)
        };

        ProcessFrame(0, local_features);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
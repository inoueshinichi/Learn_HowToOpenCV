/**
 * @file sift_keypoint_descriptor.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-10
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
        // SIFT
        

        auto local_features = [&](const cv::Mat &curFrame,
                                  const cv::Mat &lastFrame) -> void
        {
            cv::Mat img_gray;
            cv::cvtColor(curFrame, img_gray, cv::COLOR_BGR2GRAY);

            // SIFTアルゴリズム
            cv::Ptr<cv::Feature2D> detector = cv::xfeatues2d::SIFT::create();

            std::vector<cv::KeyPoint> keyPoints;

            // detector->detect(img_gray, keyPoints);

            cv::Mat localDescriptors;

            // detector->compute(img_gray, keyPoints, localDescriptors);

            // 特徴点と局所特徴量を計算
            detector->detectAndCompute(img_gray, cv::noArray(), keyPoints, localDescriptors);

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
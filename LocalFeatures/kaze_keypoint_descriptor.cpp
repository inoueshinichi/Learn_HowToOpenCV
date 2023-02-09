/**
 * @file kaze_keypoint_descriptor.cpp
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
        // KAZE
        /** @brief The KAZE constructor

            @param extended Set to enable extraction of extended (128-byte) descriptor.
            @param upright Set to enable use of upright descriptors (non rotation-invariant).
            @param threshold Detector response threshold to accept point
            @param nOctaves Maximum octave evolution of the image
            @param nOctaveLayers Default number of sublevels per scale level
            @param diffusivity Diffusivity type. DIFF_PM_G1, DIFF_PM_G2, DIFF_WEICKERT or
            DIFF_CHARBONNIER
        */

        auto local_features = [&](const cv::Mat &curFrame,
                                  const cv::Mat &lastFrame) -> void
        {
            cv::Mat img_gray;
            cv::cvtColor(curFrame, img_gray, cv::COLOR_BGR2GRAY);

            // KAZEアルゴリズム
            cv::Ptr<cv::Feature2D> detector = cv::KAZE::create();

            std::vector<cv::KeyPoint> keyPoints;

            detector->detect(img_gray, keyPoints);

            cv::Mat localDescriptors;

            detector->compute(img_gray, keyPoints, localDescriptors);

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
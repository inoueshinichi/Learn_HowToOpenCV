/**
 * @file akaze_keypoint_descriptor.cpp
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
        // AKAZE
        /** @brief The AKAZE constructor

            @param descriptor_type Type of the extracted descriptor: DESCRIPTOR_KAZE,
            DESCRIPTOR_KAZE_UPRIGHT, DESCRIPTOR_MLDB or DESCRIPTOR_MLDB_UPRIGHT.
            @param descriptor_size Size of the descriptor in bits. 0 -\> Full size
            @param descriptor_channels Number of channels in the descriptor (1, 2, 3)
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

            // AKAZEアルゴリズム
            cv::Ptr<cv::Feature2D> detector = cv::AKAZE::create();

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
/**
 * @file brisk_keypoint_descriptor.cpp
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
        // BRISK
        /** @brief The BRISK constructor

            @param thresh AGAST detection threshold score.
            @param octaves detection octaves. Use 0 to do single scale.
            @param patternScale apply this scale to the pattern used for sampling the neighbourhood of a 
            keypoint.
        */

        /** @brief The BRISK constructor for a custom pattern

            @param radiusList defines the radii (in pixels) where the samples around a keypoint are taken (for
            keypoint scale 1).
            @param numberList defines the number of sampling points on the sampling circle. Must be the same
            size as radiusList..
            @param dMax threshold for the short pairings used for descriptor formation (in pixels for keypoint
            scale 1).
            @param dMin threshold for the long pairings used for orientation determination (in pixels for
            keypoint scale 1).
            @param indexChange index remapping of the bits. 
        */

        auto local_features = [&](const cv::Mat &curFrame,
                                  const cv::Mat &lastFrame) -> void
        {
            cv::Mat img_gray;
            cv::cvtColor(curFrame, img_gray, cv::COLOR_BGR2GRAY);

            // BRISKアルゴリズム
            cv::Ptr<cv::Feature2D> detector = cv::BRISK::create();

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
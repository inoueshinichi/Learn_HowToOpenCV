/**
 * @file mser_keypoint.cpp
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
        // MSER
        /** @brief Full constructor for %MSER detector

            @param delta it compares \f$(size_{i}-size_{i-delta})/size_{i-delta}\f$
            @param min_area prune the area which smaller than minArea
            @param max_area prune the area which bigger than maxArea
            @param max_variation prune the area have similar size to its children
            @param min_diversity for color image, trace back to cut off mser with diversity less than min_diversity
            @param max_evolution  for color image, the evolution steps
            @param area_threshold for color image, the area threshold to cause re-initialize
            @param min_margin for color image, ignore too small margin
            @param edge_blur_size for color image, the aperture size for edge blur
        */

        auto local_features = [&](const cv::Mat &curFrame,
                                        const cv::Mat &lastFrame) -> void
        {
            cv::Mat img_gray;
            cv::cvtColor(curFrame, img_gray, cv::COLOR_BGR2GRAY);

            // MSERアルゴリズム
            cv::Ptr<cv::Feature2D> detector = cv::MSER::create();

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
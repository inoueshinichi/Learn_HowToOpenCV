/**
 * @file rotation.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

#if _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <cmath>

int main(int argc, char **argv)
{
    try
    {
        // テスト画像
        std::string test_file = GetTestData("lena.jpg");
        std::cout << "Test file path: " << test_file << std::endl;

        // 画像読み込み
        cv::Mat img_src;
        img_src = cv::imread(test_file, cv::IMREAD_GRAYSCALE);
        if (img_src.empty())
            throw("failed open file.");
        CV_IMSHOW(img_src)

        // 回転
        cv::Mat img_rot;
        float angle = 30;
        cv::Point2f center(img_src.cols / 2, img_src.rows / 2);
        cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0); // angle, scale
        cv::warpAffine(img_src, img_rot, rot, img_src.size(), cv::INTER_CUBIC, cv::BORDER_REPLICATE);

        CV_IMSHOW(img_rot)

        cv::waitKey(0);
    }
    catch (const char **str)
    {
        std::cerr << str << std::endl;
    }

    return 0;
}
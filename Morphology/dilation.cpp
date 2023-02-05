/**
 * @file dilation.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
    try
    {
        // テスト画像
        std::string test_file = GetTestData("nekosan.jpg");
        std::cout << "Test file path: " << test_file << std::endl;

        // 画像読み込み
        cv::Mat img_src;
        img_src = cv::imread(test_file, cv::IMREAD_GRAYSCALE);
        if (img_src.empty())
            throw("failed open file.");
        cv::imshow("img_src", img_src);

        // 画像準備
        cv::Mat img_dst1, img_dst2;

        // ここに処理を記述

        // 通常の２値化処理
        cv::Mat img_bin;
        int thresh = 100;
        threshold(img_src, img_bin, thresh, 255, cv::THRESH_BINARY);
        cv::imshow("img_bin", img_bin);

        // 膨張(dilation)
        cv::Mat element4 = (cv::Mat_<uchar>(3, 3)
                                << 0,
                            1, 0, 1, 1, 1, 0, 1, 0); // 4近傍
        cv::Mat element8 = (cv::Mat_<uchar>(3, 3)
                                << 1,
                            1, 1, 1, 1, 1, 1, 1, 1); // 8近傍

        // 4近傍
        cv::dilate(img_bin, img_dst1, element4, cv::Point(-1, -1), 1);
        // 8近傍
        cv::dilate(img_bin, img_dst2, element8, cv::Point(-1, -1), 1);

        cv::imshow("img_dst1", img_dst1);
        cv::imshow("img_dst2", img_dst2);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
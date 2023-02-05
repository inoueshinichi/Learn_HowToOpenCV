/**
 * @file laplacian_filter.cpp
 * @author your name (you@domain.com)
 * @brief ラプラシアンフィルタ
 * @version 0.1
 * @date 2021-06-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

int main(int argc, char** argv)
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
        cv::Mat img_dst, img_laplacian;
        img_src.copyTo(img_dst);
        img_src.copyTo(img_laplacian);

        // ラプラシアンフィルタ
        cv::Laplacian(img_src, img_laplacian, CV_32F, 3);
        cv::convertScaleAbs(img_laplacian, img_dst, 1, 0);
        cv::imshow("img_dst", img_dst);


        cv::waitKey(0);
    }
    catch (const char* str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
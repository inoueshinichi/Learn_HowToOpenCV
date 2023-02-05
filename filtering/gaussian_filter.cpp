/**
 * @file gaussian_filter.cpp
 * @author your name (you@domain.com)
 * @brief ガウシアンフィルタ
 * @version 0.1
 * @date 2021-06-01
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
        img_src = cv::imread(argv[1], IMREAD_GRAYSCALE);
        if (img_src.empty())
            throw ("failed open file.");

        // 画像準備
        cv::Mat img_dst;
        img_src.copyTo(img_dst);
        

        // ここに処理を記述

        // ガウシアンオペレータによる画像平滑化
        cv::GaussianBlur(img_src, img_dst, Size(11, 11), 1);
        cv::imshow("img_dst", img_dst);

        cv::waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
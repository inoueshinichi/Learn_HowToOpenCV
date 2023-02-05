/**
 * @file organization_dithering.cpp
 * @author your name (you@domain.com)
 * @brief 組織的ディザリング
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
        img_src = cv::imread(test_file, cv::IMREAD_GRAYSCALE);
        if (img_src.empty())
            throw("failed open file.");
        cv::imshow("img_src", img_src);

        // 画像準備
        cv::Mat img_dst;
        img_src.copyTo(img_dst);

        // ここに処理を記述

        /* 組織的ディザリング */
        const int N = 4;
        // Bayer型ディザ行列
        int matrix[N][N] = {{ 0,  8,  2, 10},
                            {12,  4, 14,  6},
                            { 3, 11,  1,  9},
                            {15,  7, 13,  5}};
        // matrixを0-255に変換
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] *= N * N;
            }
        }
        for (int y = 0; y < img_src.rows; y++) {
            for (int x = 0; x < img_src.cols; x++) {
                int v = img_src.data[y * img_src.step + x];
                if (v < matrix[y%N][x%N])
                    img_dst.data[y * img_dst.step + x] = 0;
                else
                    img_dst.data[y * img_dst.step + x] = 255;
            }
        }

        cv::imshow("img_dst", img_dst);

        cv::waitKey(0);
    }
    catch (const char* str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
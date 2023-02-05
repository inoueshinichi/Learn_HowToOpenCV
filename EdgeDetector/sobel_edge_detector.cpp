/**
 * @file sobel_filter.cpp
 * @author your name (you@domain.com)
 * @brief Sobelフィルタ
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

#if defined(MY_SOBEL)
        // 画像準備
        cv::Mat img_dst1, img_dst2, img_dst3, img_dst4;
        img_src.copyTo(img_dst1);
        img_src.copyTo(img_dst2);
        img_src.copyTo(img_dst3);
        img_src.copyTo(img_dst4);

        // Sobelオペレータによるエッジ検出
        double sobel_wop1[3][3] = {
            { -1.0, 0.0, 1.0 },
            { -2.0, 0.0, 2.0 },
            { -1.0, 0.0, 1.0 }
        };
        double sobel_wop2[3][3] = {
            { 1.0, 0.0, -1.0 },
            { 2.0, 0.0, -2.0 },
            { 1.0, 0.0, -1.0 }
        };
        double sobel_hop1[3][3] = {
            { -1.0, -2.0, -1.0 },
            { 0.0, 0.0, 0.0 },
            { 1.0, 2.0, 1.0 }
        };
        double sobel_hop2[3][3] = {
            { 1.0, 2.0, 1.0 },
            { 0.0, 0.0, 0.0 },
            { -1.0, -2.0, -1.0}
        };


        // 面倒くさいのでフィルタが画像からはみ出る場合は、処理しない方針とする
        int k, l;
        int sum1, sum2, sum3, sum4;
        for (int y = 0; y < img_src.rows; ++y) {
            for (int x = 0; x < img_src.cols; ++x) {
                sum1 = sum2 = sum3 = sum4;
                for (k = -1; k <=1; ++k) {
                    if (y+k<0) continue;
                    if (y+k>=img_src.rows) continue;
                    for (l = -1; l<=1; ++l) {
                        if (x+l<0) continue;
                        if (x+l>=img_src.cols) continue;
                        
                        int tx = x+l;
                        int ty = y+k;

                        sum1 += img_src.data[ty*img_src.step + tx] * sobel_wop1[k+1][l+1];
                        sum2 += img_src.data[ty*img_src.step + tx] * sobel_wop2[k+1][l+1];
                        sum3 += img_src.data[ty*img_src.step + tx] * sobel_hop1[k+1][l+1];
                        sum4 += img_src.data[ty*img_src.step + tx] * sobel_hop2[k+1][l+1];
                        
                        // 本来は+/-の符号付き画素値になるが画像として見やすいように絶対値に変換する
                        sum1 = abs(sum1);
                        if (sum1 > 255) sum1 = 255;
                        sum2 = abs(sum2);
                        if (sum2 > 255) sum2 = 255;
                        sum3 = abs(sum3);
                        if (sum3 > 255) sum3 = 255;
                        sum4 = abs(sum4);
                        if (sum4 > 255) sum4 = 255;
                    }
                }
                // 計算結果を格納
                img_dst1.data[y*img_dst1.step + x] = sum1;
                img_dst2.data[y*img_dst2.step + x] = sum2;
                img_dst3.data[y*img_dst3.step + x] = sum3;
                img_dst4.data[y*img_dst4.step + x] = sum4;
            }
        }
        imshow("img_dst1", img_dst1);
        imshow("img_dst2", img_dst2);
        imshow("img_dst3", img_dst3);
        imshow("img_dst4", img_dst4);
#endif

        // OpenCV: Sobelフィルタ
        cv::Mat img_sobel, img_dst;
        img_src.copyTo(img_sobel);
        
        cv::Sobel(img_src, img_sobel, CV_32F, 1, 0, 3);
        cv::convertScaleAbs(img_sobel, img_dst, 1, 0);
        cv::imshow("img_dst", img_dst);

        cv::waitKey(0);
    }
    catch (const char* str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
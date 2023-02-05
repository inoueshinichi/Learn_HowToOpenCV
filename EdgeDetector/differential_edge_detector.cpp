/**
 * @file differential_filter.cpp
 * @author your name (you@domain.com)
 * @brief 微分フィルタ
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
        cv::Mat img_dst1, img_dst2, img_dst3, img_dst4, img_dst5, img_dst6, img_dst7;
        img_src.copyTo(img_dst1);
        img_src.copyTo(img_dst2);
        img_src.copyTo(img_dst3);
        img_src.copyTo(img_dst4);
        img_src.copyTo(img_dst5);
        img_src.copyTo(img_dst6);
        img_src.copyTo(img_dst7);

         // 微分オペレータによるエッジ検出
        double width_op1[3][3] = {
            { 0.0, 0.0, 0.0 },
            { 0.0, -1.0, 1.0 },
            { 0.0, 0.0, 0.0}
        };
        double height_op1[3][3] = {
            { 0.0, 1.0, 0.0 },
            { 0.0, -1.0, 0.0 },
            { 0.0, 0.0, 0.0}
        };
        double width_op2[3][3] = {
            { 0.0, 0.0, 0.0 },
            { -1.0, 1.0, 0.0 },
            { 0.0, 0.0, 0.0}
        };
        double height_op2[3][3] = {
            { 0.0, 0.0, 0.0 },
            { 0.0, 1.0, 0.0 },
            { 0.0, -1.0, 0.0}
        };
        double width_op3[3][3] = {
            { 0.0, 0.0, 0.0 },
            { -0.5, 0.0, 0.5 },
            { 0.0, 0.0, 0.0 }
        };
        double height_op3[3][3] = {
            { 0.0, 0.5, 0.0 },
            { 0.0, 0.0, 0.0 },
            { 0.0, -0.5, 0.0 }
        };

        // 面倒くさいのでフィルタが画像からはみ出る場合は、処理しない方針とする
        int k, l;
        int sum1, sum2, sum3, sum4, sum5, sum6, sum7;
        for (int y = 0; y < img_src.rows; ++y) {
            for (int x = 0; x < img_src.cols; ++x) {
                sum1 = sum2 = sum3 = sum4 = sum5 = sum6 = sum7;
                for (k = -1; k <=1; ++k) {
                    if (y+k<0) continue;
                    if (y+k>=img_src.rows) continue;
                    for (l = -1; l<=1; ++l) {
                        if (x+l<0) continue;
                        if (x+l>=img_src.cols) continue;
                        
                        int tx = x+l;
                        int ty = y+k;

                        sum1 += img_src.data[ty*img_src.step + tx] * width_op1[k+1][l+1];
                        sum2 += img_src.data[ty*img_src.step + tx] * height_op1[k+1][l+1];
                        sum3 += img_src.data[ty*img_src.step + tx] * width_op2[k+1][l+1];
                        sum4 += img_src.data[ty*img_src.step + tx] * height_op2[k+1][l+1];
                        sum5 += img_src.data[ty*img_src.step + tx] * width_op3[k+1][l+1];
                        sum6 += img_src.data[ty*img_src.step + tx] * height_op3[k+1][l+1];
                        sum7 += sqrt(sum5*sum5 + sum6*sum6); // 上下の勾配の強さの和

                        if (sum1 < 0) sum1 = 0;
                        if (sum1 > 255) sum1 = 255;
                        if (sum2 < 0) sum2 = 0;
                        if (sum2 > 255) sum2 = 255;
                        if (sum3 < 0) sum3 = 0;
                        if (sum3 > 255) sum3 = 255;
                        if (sum4 < 0) sum4 = 0;
                        if (sum4 > 255) sum4 = 255;
                        if (sum5 < 0) sum5 = 0;
                        if (sum5 > 255) sum5 = 255;
                        if (sum6 < 0) sum6 = 0;
                        if (sum6 > 255) sum6 = 255;
                        if (sum7 < 0) sum7 = 0;
                        if (sum7 > 255) sum7 = 255;
                    }
                }
                // 計算結果を格納
                img_dst1.data[y*img_dst1.step + x] = sum1;
                img_dst2.data[y*img_dst2.step + x] = sum2;
                img_dst3.data[y*img_dst3.step + x] = sum3;
                img_dst4.data[y*img_dst4.step + x] = sum4;
                img_dst5.data[y*img_dst5.step + x] = sum5;
                img_dst6.data[y*img_dst6.step + x] = sum6;
                img_dst7.data[y*img_dst7.step + x] = sum7;
            }
        }
        cv::imshow("img_dst1", img_dst1);
        cv::imshow("img_dst2", img_dst2);
        cv::imshow("img_dst3", img_dst3);
        cv::imshow("img_dst4", img_dst4);
        cv::imshow("img_dst5", img_dst5);
        cv::imshow("img_dst6", img_dst6);
        cv::imshow("img_dst7", img_dst7);


        cv::waitKey(0);
    }
    catch (const char* str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
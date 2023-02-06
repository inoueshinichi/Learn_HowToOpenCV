/**
 * @file affine.cpp
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

int main(int argc, char** argv)
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

        double theta;

        /* アフィン変換（45度回転）*/
        cv::Mat img_affine1;
        theta = -M_PI / 4.0;
        cv::Mat affine_matrix1 = (cv::Mat_<double>(2, 3) << 
            cos(theta), -sin(theta), 0.,
            sin(theta), cos(theta), 0.);

        cv::warpAffine(img_src, img_affine1, affine_matrix1, img_src.size(), cv::INTER_CUBIC);
        CV_IMSHOW(img_affine1)

        /* アフィン変換（45度回転→y軸方向に画像の高さの半分だけ移動）*/
        cv::Mat img_affine2;
        theta = -M_PI / 4.0;
        cv::Mat affine_matrix2 = (cv::Mat_<double>(2, 3) << 
                                cos(theta), -sin(theta), 0.,
                                sin(theta), cos(theta), img_src.rows * 0.5);

        cv::warpAffine(img_src, img_affine2, affine_matrix2, img_src.size(), cv::INTER_CUBIC);
        CV_IMSHOW(img_affine2)

        /* アフィン変換 (OpenCVの関数を使って変換行列を作るパターン)*/
        cv::Mat img_affine3;
        float scale = 0.8;
        float angle = 45.0;
        // 回転中心
        cv::Point2f center(img_src.cols/2, img_src.rows/2);
        // 変換行列
        cv::Mat affine_matrix3 = cv::getRotationMatrix2D(center, angle, scale);
        cv::warpAffine(img_src, img_affine3, affine_matrix3, img_src.size(), cv::INTER_CUBIC);
        CV_IMSHOW(img_affine3)

        cv::waitKey(0);
    }
    catch(const char** str)
    {
        std::cerr << str << std::endl;
    }

    return 0;
}
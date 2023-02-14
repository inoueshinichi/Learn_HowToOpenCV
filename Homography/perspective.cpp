/**
 * @file perspective_2d.cpp
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


        /* 射影変換 */
        cv::Mat img_perspective;

        // 対応点
        cv::Point2f pts1[] = {
            cv::Point2f(111, 511), cv::Point2f(400, 511), cv::Point2f(400, 240), cv::Point2f(111, 240)};
        cv::Point2f pts2[] = {
            cv::Point2f(111, 511), cv::Point2f(400, 511), cv::Point2f(350, 240), cv::Point2f(161, 240)};

        // 射影変換行列
        cv::Mat perspective_matrix = cv::getPerspectiveTransform(pts1, pts2);
        /* 第3引数 solver
                DECOMP_LU (default)
                DECOMP_SVD
                DECOMP_EIG
                DECOMP_CHOLESKY
                DECOMP_QR
                DECOMP_NORMAL
        */

        // 射影変換
        cv::warpPerspective(img_src, img_perspective, perspective_matrix, img_src.size(), cv::INTER_LINEAR);
        CV_IMSHOW(img_perspective)


        

        cv::waitKey(0);
    }
    catch (const char **str)
    {
        std::cerr << str << std::endl;
    }

    return 0;
}
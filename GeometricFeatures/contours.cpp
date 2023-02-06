/**
 * @file geometric_features.cpp
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

int main(int argc, char **argv)
{
    try
    {
        // 画像読み込み
        std::string test_file1 = GetTestData("catmod.jpg");
        std::string test_file2 = GetTestData("cat.jpg");
        std::cout << "Test file 1 path: " << test_file1 << std::endl;
        std::cout << "Test file 2 path: " << test_file2 << std::endl;

        std::vector<cv::Mat> img_src_vec;
        img_src_vec.reserve(2);
        cv::Mat img_in;
        img_in = cv::imread(test_file1, cv::IMREAD_GRAYSCALE);
        img_src_vec.emplace_back(img_in);
        img_in = cv::imread(test_file2, cv::IMREAD_GRAYSCALE);
        img_src_vec.emplace_back(img_in);
        if (img_src_vec.empty())
            throw("failed open file.");
        std::printf("Got %ld images\n", img_src_vec.size());

        int rows1 = img_src_vec[0].rows;
        int cols1 = img_src_vec[0].cols;
        int rows2 = img_src_vec[1].rows;
        int cols2 = img_src_vec[1].cols;
        if (rows1 != rows2)
        {
            std::printf("rows1: %d, rows2: %d\n", rows1, rows2);
            throw("rows1 != rows2");
        }
        if (cols1 != cols2)
        {
            std::printf("cols1: %d, cols2: %d\n", cols1, cols2);
            throw("cols1 != cols2");
        }

        // 画像準備
        cv::Mat img_fg;               // 前景
        cv::Mat img_bg;               // 背景
        cv::Mat img_diff;             // 差分
        cv::Mat img_bin;              // 2値画像
        cv::Mat img_mask1, img_mask2; // マスク画像

        // 前景/背景 画像
        img_fg = img_src_vec[0].clone();
        img_bg = img_src_vec[1].clone();

        CV_IMSHOW(img_fg)
        CV_IMSHOW(img_bg)

        // 差分画像
        cv::absdiff(img_fg, img_bg, img_diff);
        CV_IMSHOW(img_diff)

        // 差分画像の2値化
        cv::threshold(img_diff, img_bin, 30, 255, cv::THRESH_BINARY);
        CV_IMSHOW(img_bin)

        // オープニング(ノイズ除去)
        cv::Mat element8 = (cv::Mat_<uchar>(3, 3)
                                << 1,
                            1, 1, 1, 1, 1, 1, 1, 1); // 8近傍

        // ノイズ除去 & 埋めあわせ
        cv::Mat img_bin_clear;
        int n = 1;
        cv::morphologyEx(img_bin, img_bin_clear, cv::MORPH_OPEN, element8, cv::Point(-1, -1), n);
        cv::morphologyEx(img_bin_clear, img_bin_clear, cv::MORPH_CLOSE, element8, cv::Point(-1, -1), n);
        CV_IMSHOW(img_bin_clear)

        /* OpenCV 面積・周囲長・円形度 */
        std::vector<std::vector<cv::Point>> contours; // 輪郭

        /*輪郭抽出モード
            RETR_EXTERNAL : 最も外側の輪郭のみを抽出
            RETR_LIST : すべての輪郭を抽出するが, 階層構造を保持しない.
            RETR_CCOMP : すべての輪郭を抽出し, 2階層構造として保存する.
            RETR_TREE : すべての輪郭を抽出し, 完全な階層構造を保存する.
        */

        /*輪郭の近似手法(一部)
            CHAIN_APPROX_NONE : すべての輪郭点を完全に格納.
            CHAIN_APPROX_SIMPLE : 線分の端点のみを残す.
        */

        cv::findContours(img_bin_clear, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        double area;
        double perimeter;
        double roundness;
        int blob_counter = 0;
        for (const auto& contour : contours)
        {
            area = cv::contourArea(cv::Mat(contour));
            perimeter = cv::arcLength(cv::Mat(contour), true);
            roundness = 4 * M_PI / perimeter / perimeter;
            std::printf("Blob[%d]: area %f, perimeter %f, roundness %f\n", blob_counter++, area, perimeter, roundness);
        }
        

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
/**
 * @file binarize.cpp
 * @author your name (you@domain.com)
 * @brief 二値化
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
        CV_IMSHOW(img_src)

        // 画像準備
        cv::Mat img_bin, img_bin_inv, img_bin_trunc, img_bin_tozero, img_bin_tozero_inv, img_masked;
        
        // 通常の２値化処理
        int thresh = 100;
        cv::threshold(img_src, img_bin, thresh, 255, cv::THRESH_BINARY);
        CV_IMSHOW(img_bin)

        // 反転２値化処理
        cv::threshold(img_src, img_bin_inv, thresh, 255, cv::THRESH_BINARY_INV);
        CV_IMSHOW(img_bin_inv)
        
        // 入力画像の画素値がしきい値より大きい場合はしきい値、
        // それ以外の場合は入力画像のまま。２値化処理
        cv::threshold(img_src, img_bin_trunc, thresh, 255, cv::THRESH_TRUNC);
        CV_IMSHOW(img_bin_trunc)

        // 入力画像の画素値がしきい値より大きい場合は
        // 入力画像のまま.それ以外は０となる
        cv::threshold(img_src, img_bin_tozero, thresh, 255, cv::THRESH_TOZERO);
        CV_IMSHOW(img_bin_tozero)

        // 入力画像の画素値がしきい値より大きい場合は０.
        // それ以外は入力画像のままとなる.
        cv::threshold(img_src, img_bin_tozero_inv, thresh, 255, cv::THRESH_TOZERO_INV);
        CV_IMSHOW(img_bin_tozero_inv)

        // マスク処理
        cv::Mat img_mask = cv::Mat::zeros(img_src.rows, img_src.cols, CV_8U);
        for (int y = (int)(img_mask.rows*0.3); y < (int)(img_mask.rows*0.5); y++) {
            for (int x = (int)(img_mask.cols*0.3); x < (int)(img_mask.cols*0.5); x++) {
                img_mask.data[y * img_mask.step + x] = 255;
            }
        }
        CV_IMSHOW(img_mask)
        img_src.copyTo(img_masked, img_mask);
        CV_IMSHOW(img_masked)

        cv::waitKey(0);
    }
    catch (const char* str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
/**
 * @file opening_closing.cpp
 * @author your name (you@domain.com)
 * @brief 膨張、収縮、クロージング、オープニング
 * @version 0.1
 * @date 2021-06-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

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

        // 通常の２値化処理
        cv::Mat img_bin;
        int thresh = 100;
        cv::threshold(img_src, img_bin, thresh, 255, cv::THRESH_BINARY);
        imshow("img_bin", img_bin);

        cv::Mat element8 = (cv::Mat_<uchar>(3, 3)
                                << 1,
                            1, 1, 1, 1, 1, 1, 1, 1); // 8近傍

        cv::Mat img_tmp;
        int n;

        // オープニング・クロージングの組み合わせによるノイズ除去
        // オープニング１回　クロージング１回
        cv::Mat img_o1c1;
        n = 1;
        cv::morphologyEx(img_bin, img_tmp, cv::MORPH_OPEN, element8, cv::Point(-1, -1), n);
        cv::morphologyEx(img_tmp, img_o1c1, cv::MORPH_CLOSE, element8, cv::Point(-1, -1), n);
        cv::imshow("img_o1c1", img_o1c1);


        // オープニング５回　クロージング５回
        cv::Mat img_o5c5;
        n = 5;
        cv::morphologyEx(img_bin, img_tmp, cv::MORPH_OPEN, element8, cv::Point(-1, -1), n);
        cv::morphologyEx(img_tmp, img_o5c5, cv::MORPH_CLOSE, element8, cv::Point(-1, -1), n);
        cv::imshow("img_o5c5", img_o5c5);

        // オープニング10回　クロージング10回
        cv::Mat img_o10c10;
        n = 10;
        cv::morphologyEx(img_bin, img_tmp, cv::MORPH_OPEN, element8, cv::Point(-1, -1), n);
        cv::morphologyEx(img_tmp, img_o10c10, cv::MORPH_CLOSE, element8, cv::Point(-1, -1), n);
        cv::imshow("img_o10c10", img_o10c10);

        cv::waitKey(0);
    }
    catch (const char* str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
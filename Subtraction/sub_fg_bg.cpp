/**
 * @file sub_fg_bg.cpp
 * @author your name (you@domain.com)
 * @brief 前景画像と背景画像の処理
 * @version 0.1
 * @date 2021-06-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

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
            throw ("rows1 != rows2");
        }
        if (cols1 != cols2)
        {
            std::printf("cols1: %d, cols2: %d\n", cols1, cols2);
            throw ("cols1 != cols2");
        }

        // 画像準備
        cv::Mat img_fg; // 前景
        cv::Mat img_bg; // 背景
        cv::Mat img_diff; // 差分
        cv::Mat img_bin; // 2値画像
        cv::Mat img_mask1, img_mask2; // マスク画像
        
        // 前景/背景 画像
        img_fg = img_src_vec[0].clone();
        img_bg = img_src_vec[1].clone();
        // img_src_vec[0].copyTo(img_fg);
        // img_src_vec[1].copyTo(img_bg);

        CV_IMSHOW(img_fg)
        CV_IMSHOW(img_bg)

        // 差分画像
        cv::absdiff(img_fg, img_bg, img_diff);
        CV_IMSHOW(img_diff)

        // 差分画像の2値化
        cv::threshold(img_diff, img_bin, 30, 255, cv::THRESH_BINARY);
        CV_IMSHOW(img_bin)

        // マスク画像
        int n = 4;
        cv::dilate(img_bin, img_mask1, cv::Mat(), cv::Point(-1, -1), n);
        cv::erode(img_mask1, img_mask2, cv::Mat(), cv::Point(-1, -1), n);

        // 前景画像から抽出
        cv::Mat img_masked2_fg;
        cv::bitwise_and(img_fg, img_mask2, img_masked2_fg);
        CV_IMSHOW(img_masked2_fg)

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
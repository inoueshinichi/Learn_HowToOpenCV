/**
 * @file split_merge.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv)
{
    try
    {
        // テスト画像
        std::string test_file = GetTestData("nekosan.jpg");
        std::cout << "Test file path: " << test_file << std::endl;

        // 画像読み込み
        cv::Mat img_src;
        img_src = cv::imread(test_file, cv::IMREAD_COLOR); // BGR
        if (img_src.empty())
            throw("failed open file.");
        cv::imshow("img_src", img_src);

        // 画像準備
        cv::Mat img_rgb;

        std::vector<cv::Mat> img_bgr(3);
        cv::split(img_src, img_bgr); // チャネル毎に分割

        // 青->赤, 緑->青, 赤->緑 に変更
        cv::merge(std::vector<cv::Mat>{img_bgr[1], img_bgr[2], img_bgr[0]}, img_rgb);

        CV_IMSHOW(img_rgb) // cv::imshow()はbgrで表示するのでrgb形式で表示すると変になる.

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
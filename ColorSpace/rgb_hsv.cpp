/**
 * @file rgb_hsv.cpp
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
        img_src = cv::imread(test_file, cv::IMREAD_COLOR);
        if (img_src.empty())
            throw("failed open file.");
        cv::imshow("img_src", img_src);

        cv::Mat img_hsv;
        cv::Mat img_bgr;

        /**
         * @brief HSV
         * hue(色相), saturation(彩度), value(明度)
         * 明るさの変動を受けにくく, 特定の色を抽出したり, 色合いを変えたりするなどが容易になる.
         * 画像内の明度変動は大きいが, 色相, 彩度は変化しにくい点が画像処理に利用できる.
         * 
         * 
         */

#if defined(MY_RGB_HSV)

#else
        cv::cvtColor(img_src, img_hsv, cv::COLOR_BGR2HSV);
        cv::cvtColor(img_hsv, img_bgr, cv::COLOR_HSV2BGR);
#endif

        CV_IMSHOW(img_hsv)
        CV_IMSHOW(img_bgr)

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
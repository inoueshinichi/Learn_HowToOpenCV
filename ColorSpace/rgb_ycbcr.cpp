/**
 * @file rgb_ycbcr.cpp
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

        cv::Mat img_ycbcr;
        cv::Mat img_bgr;

        /**
         * @brief YCbCr
         * (*RGBは[0,255])
         * Y = 0.257R + 0.504G + 0.098B + 16
         * Cb = -0.148R - 0.291B + 0.439B + 128
         * Cr = 0.439R - 0.368G - 0.071B + 128
         * 
         * (*RGBは[0,255])
         * R = 1.164(Y-16) + 1.596(Cr-128)
         * G = 1.164(Y-16) - 0.391(Cb-128) - 0.813(Cr-128)
         * B = 1.164(Y-16) + 2.018(Cb-128)
         */

#if defined(MY_RGB_YCBCR)

#else
        cv::cvtColor(img_src, img_ycbcr, cv::COLOR_BGR2YCrCb);
        cv::cvtColor(img_ycbcr, img_bgr, cv::COLOR_YCrCb2BGR);
#endif

        CV_IMSHOW(img_ycbcr)
        CV_IMSHOW(img_bgr)

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
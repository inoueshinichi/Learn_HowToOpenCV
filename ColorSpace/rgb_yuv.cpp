/**
 * @file rgb2yuv.cpp
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

        // 画像準備
        cv::Mat img_yuv;
        cv::Mat img_bgr;

        /**
         * @brief YUV(YCbCr)フォーマット
         * 色情報を輝度信号(Y), 輝度信号と青色成分の差(U), 輝度信号と赤色成分の差(V)の組合せで表現する.
         * ヒトの目が変化を敏感に感じ取れる輝度信号とそうでない色差信号に分けて色情報を表現している.
         * フォーマット形式: YUV444, YUV422, YUV411 など複数のフォーマット形式がある.
         * 
         * YUV444
         * Y(8bit) + U(8bit) + V(8bit) = 24bit/pixel
         * YUV422
         * Y(8bit) + U(4bit) + V(4bit) = 16bit/pixel
         * YUV411
         * Y(8bit) + U(2bit) + V(2bit) = 12bit/pixel
         * 
         * RGB->YUV
         * (*RGBは[0,1]正規化済み)
         * Y = 0.299R + 0.587G + 0.114B
         * U = -0.169R - 0.331G + 0.500B
         * V = 0.500R - 0.419G - 0.081B
         * 
         * YUV
         * (*RGBは[0,1]範囲)
         * R = 1.000Y + 1.402V
         * G = 1.000Y - 0.344U - 0.714V
         * B = 1.000Y + 1.772U
         */

#if defined(MY_RGB_YUV)
        

#else
        cv::cvtColor(img_src, img_yuv, cv::COLOR_BGR2YUV);
        cv::cvtColor(img_yuv, img_bgr, cv::COLOR_YUV2BGR);
        
#endif
        CV_IMSHOW(img_yuv)
        CV_IMSHOW(img_bgr)

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
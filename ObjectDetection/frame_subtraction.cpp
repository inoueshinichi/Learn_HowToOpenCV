/**
 * @file frame_subtraction.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-02-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

// スルー画像の取り込み処理(拡張版)
CV_DEFINE_PROCESSFRAME_EX

int main(int argc, char **argv)
{
    try
    {
        // フレーム間差分による前景の抽出
        cv::Mat img_diff1, img_diff1_gray, img_diff1_bin;
        cv::Mat img_diff2, img_diff2_gray, img_diff2_bin;
        int threshold = 10;
        int n = 1;
        cv::Mat img_tmp_bin, img_result_bin;
        cv::Mat element8 = (cv::Mat_<uchar>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1); // 8近傍
        

        auto frame_subtraction = [&](const cv::Mat &curFrame,
                                     const cv::Mat &lastFrame,
                                     const cv::Mat &secondLastFrame) -> void
        {
            int height = curFrame.rows;
            int width = curFrame.cols;

            // フレーム間差分1
            cv::absdiff(curFrame, lastFrame, img_diff1);
            cv::cvtColor(img_diff1, img_diff1_gray, cv::COLOR_BGR2GRAY);
            cv::threshold(img_diff1_gray, img_diff1_bin, threshold, 255, cv::THRESH_BINARY);
            // CV_IMSHOW(img_diff1)
            // CV_IMSHOW(img_diff1_gray)
            // CV_IMSHOW(img_diff1_bin)

            // フレーム間差分2
            cv::absdiff(lastFrame, secondLastFrame, img_diff2);
            cv::cvtColor(img_diff2, img_diff2_gray, cv::COLOR_BGR2GRAY);
            cv::threshold(img_diff2_gray, img_diff2_bin, threshold, 255, cv::THRESH_BINARY);

            // 2つの2値化画像から共通部分を抽出
            cv::Mat img_common_bin(cv::Size(width, height), CV_8UC1);
            cv::bitwise_and(img_diff1_bin, img_diff2_bin, img_common_bin);
            // CV_IMSHOW(img_common_bin)

            // オープニング&クロージング処理
            cv::morphologyEx(img_common_bin, img_tmp_bin, cv::MORPH_OPEN, element8, cv::Point(-1, -1), n);
            cv::morphologyEx(img_tmp_bin, img_result_bin, cv::MORPH_CLOSE, element8, cv::Point(-1, -1), n);
            // CV_IMSHOW(img_result_bin)

            // 2値化画像をマスクとして前景を抽出
            cv::Mat img_fg;
            lastFrame.copyTo(img_fg, img_result_bin);
            CV_IMSHOW(img_fg)

            //CV_IMSHOW(lastFrame)
        };

        ProcessFrameEx(0, frame_subtraction);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
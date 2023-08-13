/**
 * @file tracking_meanshift.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief MeanShiftによる物体トラッキング
 * @version 0.1
 * @date 2023-08-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

// スルー画像の取り込み処理
CV_DEFINE_PROCESSFRAME

int main(int argc, char **argv)
{
    try
    {
        // 画像メモリ
        cv::Mat img_src;
        cv::Mat img_gray_prev;
        cv::Mat img_gray;
        cv::Mat img_bin;
        cv::Mat img_bin_rgb;
        cv::Mat img_hsv;
        std::vector<cv::Mat> vec_bgr(3);

        // MeanShift
        cv::Rect tracking_roi;
        tracking_roi.x = 0;
        tracking_roi.y = 0;
        tracking_roi.width = 250;
        tracking_roi.height = 250;
        cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 10, 1); // 終了条件 max-iter: 10, min-dist-pixel: < 1

        // 2値化
        // int threshold = 220;

        // HSV Thresholds
        int h_upper = 115, h_lower = 60; // H
        int s_upper = 255, s_lower = 50; // S
        int v_upper = 200, v_lower = 20; // V

        // 表示用
        cv::Mat img_raw_resized;
        cv::Mat img_bin_resized;
        cv::Mat img_dst_resized;

        auto meanshift_tracking = [&](const cv::Mat &curFrame,
                                      const cv::Mat &lastFrame) -> void
        {
            // 現フレーム画像
            img_src = curFrame;

            // cv::split(img_src, vec_bgr); // RGB分離

            // HSV変換
            cv::cvtColor(img_src, img_hsv, cv::COLOR_BGR2HSV_FULL);

            // HSVしきい値処理
            cv::inRange(img_hsv,
                        cv::Scalar(h_lower, s_lower, v_lower),
                        cv::Scalar(h_upper, s_upper, v_upper),
                        img_bin);

            // std::cout << "img_bin.rows: " << img_bin.rows << std::endl;
            // std::cout << "img_bin.cols: " << img_bin.cols << std::endl;

            /* MeanShift */
            int ret = cv::meanShift(img_bin,
                                    /*cv::Rect*/ tracking_roi,
                                    /*cv::TermCriteria*/ criteria);

            // 矩形領域を描画
            cv::Mat img_dst = img_src.clone();
            cv::rectangle(img_dst,
                          /*cv::Rect*/ tracking_roi,
                          /*const cv::Scalar&*/ cv::Scalar(255, 0, 0),
                          /*int thickness*/ 3);

            std::vector<cv::Mat> rgb_array;
            rgb_array.reserve(3);
            rgb_array.resize(3);
            rgb_array.at(0) = img_bin;
            rgb_array.at(1) = img_bin;
            rgb_array.at(2) = img_bin;

            cv::merge(rgb_array, /*cv::Mat*/ img_bin_rgb);

            // 表示用画像
            int width = curFrame.cols;
            int height = curFrame.rows;
            int half_width = (int)(width / 2);
            int half_height = (int)(height / 2);
            cv::resize(img_src, img_raw_resized, cv::Size(half_width, half_height));
            cv::resize(img_bin_rgb, img_bin_resized, cv::Size(half_width, half_height));
            cv::resize(img_dst, img_dst_resized, cv::Size(half_width, half_height));
            cv::Mat img_show = cv::Mat::zeros(cv::Size(width, height), curFrame.type());

            // 部分領域に書き込む
            img_raw_resized.copyTo(img_show(cv::Rect(0, 0, half_width, half_height)));                    // row(左上)
            img_bin_resized.copyTo(img_show(cv::Rect(half_width, 0, half_width, half_height)));           // bin(右上)
            img_dst_resized.copyTo(img_show(cv::Rect(half_width, half_height, half_width, half_height))); // dst(右下)

            CV_IMSHOW(img_show)
            // CV_IMSHOW(curFrame)
            // CV_IMSHOW(lastFrame)
        };

        ProcessFrame(0, meanshift_tracking);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
}
/**
 * @file mat_roi.cpp
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

        // ROI
        int top = 0;
        int left = 0;
        int ROI_WIDTH = 640;
        int ROI_HEIGHT = 480;

        // 表示用
        cv::Mat img_bin_rgb;
        cv::Mat img_raw_resized;
        cv::Mat img_roi_resized_1;
        cv::Mat img_roi_resized_2;
        cv::Mat img_roi_resized_3;

        auto roi_operation = [&](const cv::Mat &curFrame,
                                      const cv::Mat &lastFrame) -> void
        {
            // 現フレーム画像
            img_src = curFrame;

            // グレースケールに変換
            cv::cvtColor(lastFrame, img_gray_prev, cv::COLOR_BGR2GRAY); // 1フレーム前
            cv::cvtColor(curFrame, img_gray, cv::COLOR_BGR2GRAY);       // 現フレーム

            //------
            // ROI
            //------
            cv::Rect roi = cv::Rect(left, top, ROI_WIDTH, ROI_HEIGHT); // ROI(0, 0, 100, 100)
            // cv::Mat img_roi(img_dst, roi); // ROI-1
            cv::Mat img_roi = img_src(roi); // ROI-2

            // ROI情報を取得する
            cv::Size roi_region;
            cv::Point left_top;
            img_roi.locateROI(roi_region, left_top); // Get roi_region(100, 100), left_top(0, 0)

            // ROI領域の拡張(領域を広げたり狭めたりする)
            img_roi.adjustROI(/*dtop*/ -10,
                              /*dbottom*/ 10,
                              /*dleft*/ -10,
                              /*dright*/ 10
            ); // 右下方向にROIを10pixelずつずらす

            // 埋め込みROI2(背景黒)
            cv::Mat img_dst_1 = cv::Mat::zeros(img_src.size(), img_src.type());
            img_roi.copyTo(img_dst_1(roi)); // 埋め込み

            // 拡大縮小ROI
            cv::Mat img_dst_2 = cv::Mat::zeros(img_src.size(), img_src.type());
            img_roi.copyTo(img_dst_2); // 拡大縮小

            // 埋め込みROI2(背景黒)
            cv::Mat img_dst_3 = cv::Mat::zeros(img_src.size(), img_src.type());


            // 表示用画像
            int width = curFrame.cols;
            int height = curFrame.rows;
            int half_width = (int)(width / 2);
            int half_height = (int)(height / 2);
            cv::resize(img_src, img_raw_resized, cv::Size(half_width, half_height));
            cv::resize(img_dst_1, img_roi_resized_1, cv::Size(half_width, half_height));
            cv::resize(img_dst_2, img_roi_resized_2, cv::Size(half_width, half_height));
            cv::resize(img_dst_3, img_roi_resized_3, cv::Size(half_width, half_height));
            cv::Mat img_show = cv::Mat::zeros(cv::Size(width, height), curFrame.type());

            // 部分領域に書き込む
            img_raw_resized.copyTo(img_show(cv::Rect(0, 0, half_width, half_height)));                      // row(左上)
            img_roi_resized_1.copyTo(img_show(cv::Rect(half_width, 0, half_width, half_height)));           // roi(右上)
            img_roi_resized_2.copyTo(img_show(cv::Rect(half_width, half_height, half_width, half_height))); // roi(右下)
            img_roi_resized_3.copyTo(img_show(cv::Rect(0, half_height, half_width, half_height)));          // roi(左下)

            CV_IMSHOW(img_show)
            // CV_IMSHOW(curFrame)
            // CV_IMSHOW(lastFrame)
        };

        ProcessFrame(0, roi_operation);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
}
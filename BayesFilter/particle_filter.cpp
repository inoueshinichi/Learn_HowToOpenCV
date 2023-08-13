/**
 * @file particle_filter.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief パーティクルフィルター
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
        /**
         * @brief KalmanFilter
         *
         */

        // HSV Thresholds
        int h_upper = 115, h_lower = 60; // H
        int s_upper = 255, s_lower = 50; // S
        int v_upper = 200, v_lower = 20; // V

        // Kalman Filter Params
        cv::KalmanFilter KF(4, 2);
        KF.statePre.at<float>(0) = 0; // 初期状態変数
        KF.statePre.at<float>(1) = 0;
        KF.statePre.at<float>(2) = 0;
        KF.statePre.at<float>(3) = 0;

        // 状態遷移行列(等速直線運動) A
        KF.transitionMatrix = (cv::Mat_<float>(4, 4) << 1, 0, 1, 0,
                               0, 1, 0, 1,
                               0, 0, 1, 0,
                               0, 0, 0, 1);

        // 観測行列 H
        cv::setIdentity(KF.measurementMatrix);

        // 状態雑音 w
        cv::setIdentity(KF.processNoiseCov, cv::Scalar::all(1e-1));

        // 観測雑音 e
        cv::setIdentity(KF.measurementNoiseCov, cv::Scalar::all(1e-1));

        // 予測誤差 P, P'
        cv::setIdentity(KF.errorCovPost, cv::Scalar::all(1e-1));
        // P(k) = ((I - K(k)*H)*P'(k)
        // KF.errorCovPre P'(k) = A*P(k-1)*At + Q

        // 画像内から追跡対象を抽出するために使う画像メモリ
        cv::Mat img_hsv;
        cv::Mat img_gray;
        cv::Mat img_gray_th;
        cv::Mat img_bin;
        cv::Mat img_lbl;
        cv::Mat img_dst;
        cv::Mat img_rgb_th;

        // 表示用
        cv::Mat img_bin_rgb;
        cv::Mat img_raw_resized;
        cv::Mat img_bin_resized;
        cv::Mat img_dst_resized;

        // 8近傍
        cv::Mat element8 = (cv::Mat_<uchar>(3, 3) << 1, 1, 1,
                            1, 1, 1,
                            1, 1, 1);

        auto linear_kalman_filter = [&](const cv::Mat &curFrame,
                                        const cv::Mat &lastFrame) -> void
        {
            // 入力
            cv::Mat img_src;
            img_src = curFrame;
            std::vector<cv::Mat> vec_hsv(3);

            // 追跡対象を抽出
            cv::cvtColor(img_src, img_gray, cv::COLOR_BGR2GRAY);
            cv::cvtColor(img_src, img_hsv, cv::COLOR_BGR2HSV_FULL);
            cv::split(img_hsv, vec_hsv);

            // HSVしきい値処理
            cv::inRange(img_hsv,
                        cv::Scalar(h_lower, s_lower, v_lower),
                        cv::Scalar(h_upper, s_upper, v_upper),
                        img_bin);

            // ノイズ処理
            cv::erode(img_bin, img_bin, element8, cv::Point(-1, -1), 5);  // 収縮
            cv::dilate(img_bin, img_bin, element8, cv::Point(-1, -1), 5); // 膨張

            // 面積最大ラベルの選択
            cv::Mat stats, centroids;
            int labelnum = cv::connectedComponentsWithStats(img_bin, img_lbl, stats, centroids);
            // cout << labelnum << endl;
            if (labelnum == 1)
            {
                // continue;
                // return;
            }

            long int max_area = 0, max_index = 0;
            for (int i = 1; i < labelnum; i++)
            {
                int area = stats.at<int>(i, cv::CC_STAT_AREA);
                if (area > max_area)
                {
                    max_area = area;
                    max_index = i;
                }
            }
            cv::compare(img_lbl, max_index, img_dst, cv::CMP_EQ); // ラベル計算

            // 面積最大ラベルの重心
            cv::Moments m = cv::moments(img_dst, true);
            cv::Point pos(m.m10 / m.m00, m.m01 / m.m00);

            // カルマンフィルタ
            // 観測
            cv::Mat measurement(2, 1, CV_32F);
            measurement.at<float>(0) = pos.x;
            measurement.at<float>(1) = pos.y;

            // 修正
            cv::Mat correction = KF.correct(measurement);

            // 予測
            cv::Mat prediction = KF.predict();

            /* 結果の描画 */

            // 2値画像をMonoからRGBに変換
            cv::cvtColor(img_bin, img_bin_rgb, cv::COLOR_GRAY2BGR);

            // 出力画像
            img_dst = img_src.clone();

            // 重心位置
            cv::circle(img_dst, pos, 5, cv::Scalar(0, 0, 255), -1);
            cv::circle(img_bin_rgb, pos, 5, cv::Scalar(0, 0, 255), -1);

            // 予測位置
            cv::circle(img_dst,
                       /*cv::Point*/ cv::Point(prediction.at<float>(0), prediction.at<float>(1)),
                       /*int radius*/ 5,
                       cv::Scalar(0, 255, 255),
                       /*int thickness*/ -1);

            cv::ellipse(img_dst,
                        /*cv::Point*/ cv::Point(prediction.at<float>(0), prediction.at<float>(1)),
                        /*cv::Size axis*/ cv::Size(abs(prediction.at<float>(2)), abs(prediction.at<float>(3))),
                        /*double angle*/ 0.0,
                        /*double startAngle*/ 0.0,
                        /*double endAngle*/ 360.0,
                        cv::Scalar(0, 255, 255),
                        /*int thickness*/ 3);

            cv::circle(img_bin_rgb,
                       /*cv::Point*/ cv::Point(prediction.at<float>(0), prediction.at<float>(1)),
                       /*int radius*/ 5,
                       cv::Scalar(0, 255, 255),
                       /*int thickness*/ -1);

            cv::ellipse(img_bin_rgb,
                        /*cv::Point*/ cv::Point(prediction.at<float>(0), prediction.at<float>(1)),
                        /*cv::Size axis*/ cv::Size(abs(prediction.at<float>(2)), abs(prediction.at<float>(3))),
                        /*double angle*/ 0.0,
                        /*double startAngle*/ 0.0,
                        /*double endAngle*/ 360.0,
                        cv::Scalar(0, 255, 255),
                        /*int thickness*/ 3);

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
            img_raw_resized.copyTo(img_show(cv::Rect(0, 0, half_width, half_height)));                    // raw(左上)
            img_bin_resized.copyTo(img_show(cv::Rect(half_width, 0, half_width, half_height)));           // bin_rgb(右上)
            img_dst_resized.copyTo(img_show(cv::Rect(half_width, half_height, half_width, half_height))); // dst(右下)

            CV_IMSHOW(img_show)
            // CV_IMSHOW(curFrame)
            // CV_IMSHOW(lastFrame)
        };

        ProcessFrame(0, linear_kalman_filter);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
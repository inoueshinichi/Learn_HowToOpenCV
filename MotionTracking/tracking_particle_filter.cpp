/**
 * @file tracking_particle_filter.cpp
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
#include <BayesFilter/particle_filter.hpp>
#include <random> // std::uniform_real_distribution<float>, std::normal_distribution

// スルー画像の取り込み処理
CV_DEFINE_PROCESSFRAME

int main(int argc, char **argv)
{
    try
    {
        /**
         * @brief ParticleFilter
         *
         */
        unsigned int num_particles = 1000; // パーティクル数
        double likelihood_threshold = 0.9; // 残存パーティクル用尤度閾値
        int remaining_percentage = 10;      // 残存パーティクルの%
        unsigned int seed = 2023;
        is::ParticleFilter<is::PointCloud2dParticle> pfs(num_particles,
                                                         likelihood_threshold,
                                                         remaining_percentage,
                                                         seed);
        // 初期化フラグ
        bool initialized = false;
        double target_HUE = 70;         // ターゲットの色相
        double target_SATURATION = 200; // ターゲットの彩度

        // HSV Thresholds
        int h_upper = 115, h_lower = 60; // H
        int s_upper = 255, s_lower = 50; // S
        int v_upper = 200, v_lower = 20; // V

        // 画像内から追跡対象を抽出するために使う画像メモリ
        std::vector<cv::Mat> vec_hsv(3);
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

        auto particle_filter = [&](const cv::Mat &curFrame,
                                   const cv::Mat &lastFrame) -> void
        {
            // 入力
            cv::Mat img_src;
            img_src = curFrame;

            // 追跡対象を抽出
            cv::cvtColor(img_src, img_gray, cv::COLOR_BGR2GRAY);
            cv::cvtColor(img_src, img_hsv, cv::COLOR_BGR2HSV_FULL);
            cv::split(img_hsv, vec_hsv); // HSV分離

            // // HSVしきい値処理
            // cv::inRange(img_hsv,
            //             cv::Scalar(h_lower, s_lower, v_lower),
            //             cv::Scalar(h_upper, s_upper, v_upper),
            //             img_bin);

            // パーティクルフィルタ
            // 初期化
            if (!initialized)
            {
                auto initParticle = [&](std::mt19937 &engine) -> is::PointCloud2dParticle
                {
                    /*ここにパーティクルの初期化処理を記述*/
                    std::uniform_real_distribution<float> dist_w(0, img_src.cols); // 画像の幅まで
                    std::uniform_real_distribution<float> dist_h(0, img_src.rows); // 画像の高さまで
                    return is::PointCloud2dParticle(
                        /*pos_x*/ dist_w(engine),
                        /*pos_y*/ dist_h(engine),
                        /*vel_x*/ 0.0f,
                        /*vel_y*/ 0.0f,
                        /*likelihood*/ 1.0,
                        /*keep*/ false);
                };
                pfs.initialize(initParticle);
                initialized = true;
                std::printf("Done initialized...\n");
            }

            // 予測
            auto updateParticle = [](is::PointCloud2dParticle &p) -> void
            {
                /*ここに前フレームから次フレームへのパーティクルの予測処理を記述*/
                p._px += p._vx;
                p._py += p._vy;
            };
            pfs.predict(updateParticle);

            // 更新
            auto updateLikelihood = [&](is::PointCloud2dParticle &p) -> void
            {
                /*ここにパーティクルの尤度更新処理を記述*/
                if (0 < p._px &&
                    p._px < img_src.cols &&
                    0 < p._py &&
                    p._py < img_src.rows)
                {
                    int h = vec_hsv[0].at<unsigned char>((int)p._py, (int)p._px); // パーティクル画素の色相
                    int s = vec_hsv[1].at<unsigned char>((int)p._py, (int)p._px); // パーティクル画素の彩度
                    double len_h = std::abs(target_HUE - h);                      // Hue=70からの距離
                    double len_s = std::abs(target_SATURATION - s);               // Saturation=200からの距離

                    // 尤度
                    double likelihood = (len_h / 180) * 0.8 + (len_s / 255) * 0.2; // 尤度関数 likelihood[0,1]

                    // 最高尤度を1.0とする
                    // if (likelihood > 1.0)
                    //     likelihood = 1.0;
                    // p.setLikelihood(likelihood);
                    p.setLikelihood(1 - likelihood);
                }
                else
                {
                    p.setLikelihood(0.0);
                }
            };

            pfs.update(updateLikelihood);

            // リサンプリング
            auto genNewParticle = [&](is::PointCloud2dParticle &p, std::mt19937 &engine) -> is::PointCloud2dParticle
            {
                /*ここに新たなパーティクルを生成する処理を記述*/
                std::normal_distribution<double> ndist(0.0, img_src.cols + img_src.rows);
                std::uniform_real_distribution<double> udist(-M_PI, M_PI);

                // 移動量
                double r = ndist(engine) * (1 - p.getLikelihood());
                // 方向
                double theta = udist(engine);

                float pos_x = r * std::cos(theta) + p._px;
                float pos_y = r * std::sin(theta) + p._py;

                // 等速直線運動と仮定
                return is::PointCloud2dParticle(pos_x, pos_y,
                                                pos_x - p._px, pos_y - p._py,
                                                p.getWeight(), /*keep*/ false);
            };
            pfs.resampling(genNewParticle);

            

            /* 結果の描画 */

            // // 2値画像をMonoからRGBに変換
            // cv::cvtColor(img_bin, img_bin_rgb, cv::COLOR_GRAY2BGR);

            // 出力画像
            img_dst = img_src.clone();

            // パーティクルの描画
            const auto &particles = pfs.getParticles();
            for (int i = 0; i < particles.size(); i++)
            {
                if (0 < particles.at(i)._px &&
                    particles.at(i)._px < img_src.cols &&
                    0 < particles.at(i)._py &&
                    particles.at(i)._py < img_src.rows)
                {
                    cv::Point pos((int)particles.at(i)._px, (int)particles.at(i)._py);
                    cv::circle(img_dst, pos, /*radius*/ 2, cv::Scalar(0, 0, 255)); // 赤
                }
            }

            // 追加パーティクルの描画
            const auto &new_particles = pfs.getNewParticles();
            for (int i = 0; i < new_particles.size(); i++)
            {
                cv::Point pos((int)new_particles.at(i)._px, (int)new_particles.at(i)._py);
                cv::circle(img_dst, pos, /*radius*/ 2, cv::Scalar(255, 0, 0)); // 青
            }

            // パーティクルの重心
            double cx, cy;
            for (int i = 0; i < particles.size(); i++)
            {
                cx += particles.at(i)._px;
                cy += particles.at(i)._py;
            }
            cx /= particles.size();
            cy /= particles.size();

            // CrossLineの描画
            cv::line(img_dst, cv::Point((int)cx, 0), cv::Point((int)cx, img_src.rows), cv::Scalar(0, 255, 0), /*thickness*/3);
            cv::line(img_dst, cv::Point(0, (int)cy), cv::Point(img_src.cols, (int)cy), cv::Scalar(0, 255, 0), /*thickness*/3);

            /*パーティクルの統計*/
            std::printf("Particles: %d, NewParticles: %d\n", particles.size(), new_particles.size());

            // 表示用画像
            int width = curFrame.cols;
            int height = curFrame.rows;
            int half_width = (int)(width / 2);
            int half_height = (int)(height / 2);
            cv::resize(img_src, img_raw_resized, cv::Size(half_width, half_height));
            // cv::resize(img_bin_rgb, img_bin_resized, cv::Size(half_width, half_height));
            cv::resize(img_dst, img_dst_resized, cv::Size(half_width, half_height));
            cv::Mat img_show = cv::Mat::zeros(cv::Size(width, height), curFrame.type());

            // 部分領域に書き込む
            img_raw_resized.copyTo(img_show(cv::Rect(0, 0, half_width, half_height)));                    // raw(左上)
            // img_bin_resized.copyTo(img_show(cv::Rect(half_width, 0, half_width, half_height)));        // bin_rgb(右上)
            img_dst_resized.copyTo(img_show(cv::Rect(half_width, half_height, half_width, half_height))); // dst(右下)

            CV_IMSHOW(img_show)
            // CV_IMSHOW(curFrame)
            // CV_IMSHOW(lastFrame)
        };

        ProcessFrame(0, particle_filter, /*display_time*/true);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
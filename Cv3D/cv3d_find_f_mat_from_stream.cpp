/**
 * @file cv3d_find_f_mat_from_stream.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

// スルー画像の取り込み処理
CV_DEFINE_PROCESSFRAME

auto main(int, char**) -> int
{
    try
    {
        cv::Mat img_gray, img_gray_last;

        // AKAZEアルゴリズム
        cv::Ptr<cv::Feature2D> detector = cv::AKAZE::create();

        // マッチングアルゴリズム
        cv::Ptr<cv::DescriptorMatcher> matcher = \
            cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
        int top_k = 2;
        float match_judge = 0.6f;
        unsigned int lostCounter = 0;

        // 表示用
        cv::Mat img_cur_resized;
        cv::Mat img_last_resized;

        // 時間計算のための周波数
        double f = 1000.0 / cv::getTickFrequency();
        int64_t time_start;
        int64_t time_end;

        auto estimate_f_mat = [&](const cv::Mat &curFrame,
                                  const cv::Mat &lastFrame) -> void
        {
            time_start = cv::getTickCount();

            // Query : currentFrame
            // Trained : lastFrame

            // BGR -> Gray
            cv::cvtColor(curFrame, img_gray, cv::COLOR_BGR2GRAY);
            cv::cvtColor(lastFrame, img_gray_last, cv::COLOR_BGR2GRAY);

            std::vector<cv::KeyPoint> keyPoints, lastKeyPoints;
            cv::Mat localDescriptors, lastLocalDescriptors;

            // 特徴点＆局所特徴量
            detector->detectAndCompute(img_gray, cv::noArray(), keyPoints, localDescriptors);
            detector->detectAndCompute(img_gray_last, cv::noArray(), lastKeyPoints, lastLocalDescriptors);

            if (lastKeyPoints.size() == 0)
            {
                std::printf("Lost keypoints [%d]!\n", lostCounter++);
                if (lostCounter == std::numeric_limits<unsigned>::max())
                    lostCounter = 0;
                return;
            }

            // 特徴点のペア
            std::vector<std::vector<cv::DMatch>> matches;
            /*
               img1_match[0] : img2_match[0], img2_match[1], ..., img2_match[top_k]
               img1_match[1] : img2_match[0], img2_match[2], ..., img2_match[top_k]
                     :
                     :
               img1_match[n] : img2_match[0], img2_match[2], ..., img2_match[top_k]
            */

            // 上位k位までのマッチング(query, trained)
            matcher->knnMatch(localDescriptors, lastLocalDescriptors, matches, top_k);

            // 対応点を絞る
            std::vector<cv::DMatch> goodMatches;
            std::vector<cv::Point2f> goodPoints1; // current
            std::vector<cv::Point2f> goodPoints2; // last
            for (int i = 0; i < matches.size(); ++i)
            {
                // 比較対象の2枚の画像について, 片方の特徴点1つに対してもう片方で2点が対応している(top_k = 2)
                float dist1 = matches[i][0].distance;
                float dist2 = matches[i][1].distance;

                // 良い点を残す（最も類似する点と次に類似する点の類似度から）
                if (dist1 <= dist2 * match_judge)
                {
                    goodMatches.emplace_back(matches[i][0]);

                    int curIndex = matches[i][0].queryIdx;
                    int lastIndex = matches[i][0].trainIdx;
                    goodPoints1.emplace_back(keyPoints.at(curIndex).pt);
                    goodPoints2.emplace_back(lastKeyPoints.at(lastIndex).pt);
                }
            }

            // F(基礎行列)を求める
            /**
             * @brief F基礎行列の推定
             * @note https://docs.opencv.org/4.7.0/d9/d0c/group__calib3d.html#ga687a1ab946686f0d85ae0363b5af1d7b
             * @note method Method for computing a fundamental matrix.
             * @note FM_7POINT 7点非線形型計算 N=7
             * @note FM_8POINT 8点線形型計算 N>=8
             * @note FM_RANSAC RANSACを使って外れ値を除去 N>=8
             * @note LMedS推定を使って外れ値を除去 N>=8
             * @note ransacRepojThreshold: RANSACの場合のみ。点からエピポーラ線までの最大距離をピクセル単位で表す. デフォルト3.0.
             *       対応点の精度, 画像解像度, 画像ノイズに依存する. 範囲外の点は外れ値とみなされ, 最終的なF行列の計算から除外される.
             * @note confidence: RANSAC, LMEDSの場合のみ.推定されるF行列がどれほど正しいかを示す信頼度(確率)の要求値. デフォルト0.99.
             * @note maxIters: 最大イテレーション回数. オーバーロードされた関数では, 指定しなくてもよい.
             * @warning Normally just one matrix is found. But in case of the 7-point algorithm,
             * the function may return up to 3 solutions ( 9×3 matrix that stores all 3 matrices sequentially).
             *
             */

            // 表示用画像
            int width = curFrame.cols;
            int height = curFrame.rows;
            int half_width = (int)(width / 2);
            int half_height = (int)(height / 2);
            cv::resize(curFrame, img_cur_resized, cv::Size(half_width, half_height));
            cv::resize(lastFrame, img_last_resized, cv::Size(half_width, half_height));
            cv::Mat img_show = cv::Mat::zeros(cv::Size(width, height), curFrame.type());

            // 部分領域に書き込む
            img_cur_resized.copyTo(img_show(cv::Rect(0, 0, half_width, half_height)));                     // current(左上)
            img_last_resized.copyTo(img_show(cv::Rect(half_width, half_height, half_width, half_height))); // last(右下)

            CV_IMSHOW(img_show)

            time_end = cv::getTickCount();

            double elapsedTime = (time_end - time_start) * f;
            std::printf("Elapsed time: %d [ms], GoodMatch: %lu\n", (int)elapsedTime, goodMatches.size());
        };

        ProcessFrame(0, estimate_f_mat);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
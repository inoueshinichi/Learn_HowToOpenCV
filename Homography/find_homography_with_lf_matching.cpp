/**
 * @file find_homography_with_lf_matching.cpp
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
        // Find Homograph with AKAZE local features matching
        cv::Mat img_gray, img_gray_last;

        // AKAZEアルゴリズム
        cv::Ptr<cv::Feature2D> detector = cv::AKAZE::create();

        // マッチングアルゴリズム
        cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
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

        auto lf_matching = [&](const cv::Mat &curFrame,
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
            /* n x m = Nの特徴点ペアの内, k個抜き取る?
            img1_kp[0] : match[0,0], match[0,1], ..., match[0,top_k]
            img1_kp[1] : match[1,0], match[1,2], ..., match[1,top_k]
                    :
                    :
            img1_kp[n] : match[k,0], match[k,2], ..., match[k,top_k]
            */

            // 上位k位までのマッチング(query, trained)
            matcher->knnMatch(localDescriptors, lastLocalDescriptors, matches, top_k);

            // 対応点を絞る
            std::vector<cv::DMatch> goodMatches;
            std::vector<cv::Point2f> goodPoints1;
            std::vector<cv::Point2f> goodPoints2;

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

            // ホモグラフィ行列Hを求める
            if (goodPoints1.size() < 4 || goodPoints2.size() < 4) // 対応点が4組以上ある場合に計算可能
            {
                return;
            }
            cv::Mat H;
            cv::Mat maskH;

            // RANSACを用いた透視行列(H)の計算
            double ransacReprojThreshold = 3.0;
            H = cv::findHomography(goodPoints1, goodPoints2, maskH, cv::RANSAC, ransacReprojThreshold);

            // CV_TYPE_LOG(maskH); // CV_8UC1
            // std::printf("Shape of maskH: (%d,%d)\n", maskH.rows, maskH.cols); // (N,1)

            // RANSACで得られたインライアーのみを抽出
            std::vector<cv::DMatch> inlierMatches;
            std::vector<cv::Point2f> inlierPoints1;
            std::vector<cv::Point2f> inlierPoints2;
            for (int i = 0; i < maskH.rows; ++i)
            {
                uchar* inliner = maskH.ptr<uchar>(i);
                if (inliner[0] == 1)
                {
                    inlierMatches.emplace_back(goodMatches[i]);
                    inlierPoints1.emplace_back(goodPoints1[i]);
                    inlierPoints2.emplace_back(goodPoints2[i]);

                    // if (inlierMatches.size() == 4)
                    // {
                    //     break; // 4点のみ使う
                    // }
                }
            }

            // インライアーな対応点のみを用いて再度ホモグラフィ行列Hを計算
            H = cv::findHomography(inlierPoints1, inlierPoints2, maskH, cv::RANSAC, ransacReprojThreshold);
            // H = cv::getPerspectiveTransform(inlierPoints1, inlierPoints2, cv::DECOMP_LU); // 必ず4点のみ
            /* 第3引数 solver
                DECOMP_LU (default)
                DECOMP_SVD
                DECOMP_EIG
                DECOMP_CHOLESKY
                DECOMP_QR
                DECOMP_NORMAL
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

            if (!H.empty())
            {
                // 描画 (img_show)
                for (int k = 0; k < inlierMatches.size(); ++k)
                {
                    cv::Point2f curPoint = inlierPoints1.at(k) / 2;
                    cv::Point2f center(half_width, half_height);
                    cv::Point2f lastPoint = center + inlierPoints2.at(k) / 2;

                    // current(query)
                    cv::circle(img_show, curPoint, 3, cv::Scalar(0, 0, 255), 2);

                    // last(trained)
                    cv::circle(img_show, lastPoint, 3, cv::Scalar(0, 255, 0), 2);

                    cv::line(img_show, curPoint, lastPoint, cv::Scalar(255, 0, 0));
                }

                // 前フレーム面に対して現フレーム面の傾き度合いを四角形□(100x100)を透視変換したもので表現
                std::vector<cv::Point2f> rectPoints;
                rectPoints.reserve(4);
                rectPoints.emplace_back(cv::Point2f(0.0f, 0.0f)); // 左上
                rectPoints.emplace_back(cv::Point2f(100.0f, 0.0f)); // 右上
                rectPoints.emplace_back(cv::Point2f(100.0f, 100.0f)); // 右下
                rectPoints.emplace_back(cv::Point2f(0.0f, 100.0f)); // 右下

                std::vector<cv::Point2f> rectHPoints;
                rectHPoints.reserve(4);
                cv::perspectiveTransform(rectPoints, rectHPoints, H); // 点の透視変換

                // 描画用に平行移動
                std::for_each(rectHPoints.begin(), rectHPoints.end(), [&width, &height](cv::Point2f& pt) {
                    pt.x += width / 4;
                    pt.y += height / 4;
                });

                // 描画
                cv::line(img_show, (cv::Point)(rectHPoints[0]), (cv::Point)(rectHPoints[1]), cv::Scalar(0, 255, 0));
                cv::line(img_show, (cv::Point)(rectHPoints[1]), (cv::Point)(rectHPoints[2]), cv::Scalar(0, 255, 0));
                cv::line(img_show, (cv::Point)(rectHPoints[2]), (cv::Point)(rectHPoints[3]), cv::Scalar(0, 255, 0));
                cv::line(img_show, (cv::Point)(rectHPoints[3]), (cv::Point)(rectHPoints[0]), cv::Scalar(0, 255, 0));
            }

            

            CV_IMSHOW(img_show)

            time_end = cv::getTickCount();

            double elapsedTime = (time_end - time_start) * f;
            std::printf("Elapsed time: %d [ms], GoodMatch: %lu\n", (int)elapsedTime, goodMatches.size());
        };

        ProcessFrame(0, lf_matching);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
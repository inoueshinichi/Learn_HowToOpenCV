/**
 * @file local_feature_matching.cpp
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
        // Local Feature Matching with AKAZE
        cv::Mat img_gray, img_gray_last;

        // AKAZEアルゴリズム
        cv::Ptr<cv::Feature2D> detector = cv::AKAZE::create();

        // マッチングアルゴリズム
        cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
        int top_k = 2;
        float match_judge = 0.6f;
        unsigned int lostCounter = 0;

        typedef struct MatchPair_tag
        {
            cv::DMatch dmatch;
            cv::Point2f pt1;
            cv::Point2f pt2;
        } MatchPair;
        MatchPair matchPair;

        // 表示用
        cv::Mat img_cur_resized;
        cv::Mat img_last_resized;

        // 時間計算のための周波数
        double f = 1000.0 / cv::getTickFrequency();
        int64_t time_start;
        int64_t time_end;

        auto lf_matching = [&](const cv::Mat& curFrame,
                               const cv::Mat& lastFrame) -> void
        {
            time_start = cv::getTickCount();

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
                if (lostCounter == std::numeric_limits<unsigned>::max()) lostCounter = 0;
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
            std::vector<MatchPair> goodMatches;
            
            for (int i = 0; i < matches.size(); ++i)
            {
                // 比較対象の2枚の画像について, 片方の特徴点1つに対してもう片方で2点が対応している(top_k = 2)
                float dist1 = matches[i][0].distance;
                float dist2 = matches[i][1].distance;

                // 良い点を残す（最も類似する点と次に類似する点の類似度から）
                if (dist1 <= dist2 * match_judge)
                {
                    matchPair.dmatch = matches[i][0];
                    int curIndex = matches[i][0].queryIdx;
                    int lastIndex = matches[i][0].trainIdx;
                    matchPair.pt1 = keyPoints.at(curIndex).pt;
                    matchPair.pt2 = lastKeyPoints.at(lastIndex).pt;

                    goodMatches.emplace_back(matchPair);
                }
            }

            // 表示用画像
            int width = curFrame.cols;
            int height = curFrame.rows;
            int half_width = (int)(width / 2);
            int half_height = (int)(height / 2);
            cv::resize(curFrame, img_cur_resized, cv::Size(half_width, half_height));
            cv::resize(lastFrame, img_last_resized, cv::Size(half_width, half_height));
            cv::Mat img_show = cv::Mat::zeros(cv::Size(width, height), curFrame.type());

            // 部分領域に書き込む
            img_cur_resized.copyTo(img_show(cv::Rect(0, 0, half_width, half_height))); // current(左上)
            img_last_resized.copyTo(img_show(cv::Rect(half_width, half_height, half_width, half_height))); // last(右下)

            // 描画 (img_show)
            for (const auto &goodMatch : goodMatches)
            {
                cv::Point2f curPoint = goodMatch.pt1 / 2;
                cv::Point2f center(half_width, half_height);
                cv::Point2f lastPoint = center + goodMatch.pt2 / 2;
                
                // current(query)
                cv::circle(img_show, curPoint, 3, cv::Scalar(0, 0, 255), 2);

                // last(trained)
                cv::circle(img_show, lastPoint, 3, cv::Scalar(0, 255, 0), 2);

                cv::line(img_show, curPoint, lastPoint, cv::Scalar(255, 0, 0));
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
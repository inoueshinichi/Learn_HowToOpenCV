/**
 * @file fast_corner_detector.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-02-09
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
        // FAST
        int threshold = 30; // チェックする対象ピクセルと，それを中心とする円周上のピクセルとの比較時の閾値の上下幅
        // 　この値が小さくなると，対象ピクセルが周囲と異なる明度と見やしやすくなり，より多くのコーナーが検出されます．
        bool non_maximum_suppression = true;

        auto local_features = [&](const cv::Mat &curFrame,
                                  const cv::Mat &lastFrame) -> void
        {
            cv::Mat img_gray;
            cv::cvtColor(curFrame, img_gray, cv::COLOR_BGR2GRAY);

            // FASTアルゴリズムを使ったコーナー検出
            // cv::FAST(img_gray, keyPoints, threshold, non_maximum_suppression);
            cv::Ptr<cv::Feature2D> detector = cv::FastFeatureDetector::create(threshold,
                                                                              non_maximum_suppression);

            std::vector<cv::KeyPoint> keyPoints;

            detector->detect(img_gray, keyPoints);

            for (int i = 0; i < keyPoints.size(); i++)
            {
                cv::circle(curFrame,
                           cv::Point(keyPoints[i].pt.x, keyPoints[i].pt.y),
                           3,
                           cv::Scalar(0, 0, 255),
                           2);
            }

            CV_IMSHOW(curFrame)
            // CV_IMSHOW(lastFrame)
        };

        ProcessFrame(0, local_features);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
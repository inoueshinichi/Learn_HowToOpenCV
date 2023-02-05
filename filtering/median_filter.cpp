/**
 * @file median_filter.cpp
 * @author your name (you@domain.com)
 * @brief メディアンフィルタ(単純バブルソートで中央値を見つけているので遅すぎる)
 * @version 0.1
 * @date 2021-06-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        // テスト画像
        std::string test_file = GetTestData("nekosan.jpg");
        std::cout << "Test file path: " << test_file << std::endl;

        // 画像読み込み
        cv::Mat img_src;
        img_src = cv::imread(test_file, cv::IMREAD_GRAYSCALE);
        if (img_src.empty())
            throw("failed open file.");
        cv::imshow("img_src", img_src);

        // 画像準備
        cv::Mat img_dst;
        img_src.copyTo(img_dst);

        // ここに処理を記述

#if defined(MY_MEDIAN)
        // メディアンフィルタによる画像平滑化
        int val[9], tmp;
        int k, l;
        double sum;
        for (int y = 0; y < img_src.rows; y++) {
            for (int x = 0; x < img_src.cols; x++) {
                // 注目画素の近傍画素値を配列val[]にセット
                int i = 0;
                for (k = -1; k <= 1; k++) {
                    if (y + k < 0) continue;
                    if (y + k >= img_src.rows) continue;
                    for (l = -1; l <= 1; l++) {
                        if (x + l < 0) continue;
                        if (x + l >= img_src.cols) continue;
                        val[i++] = img_src.data[(y + k) * img_src.step + (x + l)];
                    }
                }
                // バブルソート（昇順）1, 2, 3, 4, 5, ...
                for (int i = 0; i < 8; i++) {
                    for (int j = 8; j > i; j--) {
                        if (val[j-1] > val[j]) {
                            tmp = val[j-1];
                            val[j-1] = val[j];
                            val[j] = tmp;
                        }
                    }
                }
                
                // 中央値（val[4]）を注目画素の値とする
                img_dst.data[y * img_dst.step + x] = val[4];
            }
        }
#endif

        cv::medianBlur(img_src, img_dst, 5);
        cv::imshow("img_dst", img_dst);

        cv::waitKey(0);
    }
    catch (const char* str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
/**
 * @file sub_fg_bg.cpp
 * @author your name (you@domain.com)
 * @brief 前景画像と背景画像の処理
 * @version 0.1
 * @date 2021-06-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <cstdio>
#include <iostream>
#include <string>
#include <cmath>
#include <random>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
            throw("few parameters.");

        // 画像読み込み
        vector<Mat> img_src_vec;
        for (int i = 1; i < argc; ++i)
        {
            Mat img_src = imread(argv[i], IMREAD_GRAYSCALE);

            if (img_src.empty())
                throw("failed open file.");

            img_src_vec.push_back(img_src);
        }
        std::printf("Got %d images\n", img_src_vec.size());

        // 画像準備
        Mat img_dst;
        Mat img_fg; // 前景
        Mat img_bg; // 背景
        Mat img_diff; // 差分
        Mat img_binary; // 2値画像
        Mat img_mask1, img_mask2; // マスク画像
        
        // 前景/背景 画像
        img_src_vec[0].copyTo(img_fg);
        img_src_vec[1].copyTo(img_bg);

        // 差分画像
        absdiff(img_fg, img_bg, img_diff);

        // 差分画像の2値化
        threshold(img_diff, img_binary, 50, 255, THRESH_BINARY);

        // マスク画像
        dilate(img_binary, img_mask1, Mat(), Point(-1, -1), 4);
        erode(img_mask1, img_mask2, Mat(), Point(-1, -1), 4);

        // 前景画像から抽出
        bitwise_and(img_fg, img_mask2, img_dst);

        imshow("img_fg", img_fg);
        imshow("img_bg", img_bg);
        imshow("img_mask1", img_mask1);
        imshow("img_mask2", img_mask2);
        imshow("img_dst", img_dst);

        waitKey(0);
    }
    catch (const char *str)
    {
        cerr << str << endl;
    }
    return 0;
}
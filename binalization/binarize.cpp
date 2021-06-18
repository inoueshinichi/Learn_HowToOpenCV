/**
 * @file binarize.cpp
 * @author your name (you@domain.com)
 * @brief 二値化
 * @version 0.1
 * @date 2021-06-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <iostream>
#include <string>
#include <cmath>
#include <random>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    try
    {
        if (argc < 2)
            throw ("few parameters.");

        // 画像読み込み
        Mat img_src;
        img_src = imread(argv[1], IMREAD_GRAYSCALE);
        if (img_src.empty())
            throw ("failed open file.");

        // 画像準備
        Mat img_dst1, img_dst2, img_dst3, img_dst4, img_dst5, img_dst6;
        img_src.copyTo(img_dst1);
        img_src.copyTo(img_dst2);
        img_src.copyTo(img_dst3);
        img_src.copyTo(img_dst4);
        img_src.copyTo(img_dst5);
        img_src.copyTo(img_dst6);

        // 通常の２値化処理
        int thresh = 100;
        threshold(img_src, img_dst1, thresh, 255, THRESH_BINARY);
        imshow("img_dst1", img_dst1);

        // 反転２値化処理
        threshold(img_src, img_dst2, thresh, 255, THRESH_BINARY_INV);
        imshow("img_dst2", img_dst2);
        
        // 入力画像の画素値がしきい値より大きい場合はしきい値、
        // それ以外の場合は入力画像のまま。２値化処理
        threshold(img_src, img_dst3, thresh, 255, THRESH_TRUNC);
        imshow("img_dst3", img_dst3);

        // 入力画像の画素値がしきい値より大きい場合は
        // 入力画像のまま.それ以外は０となる
        threshold(img_src, img_dst4, thresh, 255, THRESH_TOZERO);
        imshow("img_dst4", img_dst4);

        // 入力画像の画素値がしきい値より大きい場合は０.
        // それ以外は入力画像のままとなる.
        threshold(img_src, img_dst5, thresh, 255, THRESH_TOZERO_INV);
        imshow("img_dst5", img_dst5);

        // マスク処理
        Mat img_mask = Mat::zeros(img_src.rows, img_src.cols, CV_8U);
        for (int y = (int)(img_mask.rows*0.3); y < (int)(img_mask.rows*0.5); y++) {
            for (int x = (int)(img_mask.cols*0.3); x < (int)(img_mask.cols*0.5); x++) {
                img_mask.data[y * img_mask.step + x] = 255;
            }
        }
        imshow("img_mask", img_mask);
        img_src.copyTo(img_dst6, img_mask);
        imshow("img_dst6", img_dst6);

        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
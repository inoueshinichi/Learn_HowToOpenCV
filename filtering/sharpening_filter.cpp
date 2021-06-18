/**
 * @file sharpening_filter.cpp
 * @author your name (you@domain.com)
 * @brief 鮮鋭化フィルタ
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
        Mat img_sharpening;
        img_src.copyTo(img_sharpening);

        // 鮮鋭化オペレータの作成
        float k = 1.0;
        Mat op = Mat::ones(3, 3, CV_32F) * -k;
        op.at<float>(1,1) = 1 + 8*k;

        // 鮮鋭化フィルタ
        // 設定したオペレータでフィルタ
        filter2D(img_src, img_sharpening, CV_32F, op);
        convertScaleAbs(img_sharpening, img_sharpening, 1, 0);
        imshow("img_sharpening", img_sharpening);

        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
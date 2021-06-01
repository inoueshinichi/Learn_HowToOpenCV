/**
 * @file gaussian_filter.cpp
 * @author your name (you@domain.com)
 * @brief ガウシアンフィルタ
 * @version 0.1
 * @date 2021-06-01
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
        Mat img_dst;
        img_src.copyTo(img_dst);
        

        // ここに処理を記述

        // ガウシアンオペレータによる画像平滑化
        GaussianBlur(img_src, img_dst, Size(11, 11), 1);
        imshow("img_dst", img_dst);

        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
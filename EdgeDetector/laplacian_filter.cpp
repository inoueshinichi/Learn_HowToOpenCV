/**
 * @file laplacian_filter.cpp
 * @author your name (you@domain.com)
 * @brief ラプラシアンフィルタ
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
        Mat img_laplacian;
        img_src.copyTo(img_laplacian);

        // ラプラシアンフィルタ
        Laplacian(img_src, img_laplacian, CV_32F, 3);
        convertScaleAbs(img_laplacian, img_laplacian, 1, 0);
        imshow("img_laplacian", img_laplacian);


        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
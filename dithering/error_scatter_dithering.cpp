/**
 * @file error_scatter_dithering.cpp
 * @author your name (you@domain.com)
 * @brief 誤差拡散ディザリング
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

        /* 誤差拡散ディザリング */
        int thresh = 128;
        int error = 0;
        for (int y = 0; y < img_dst.rows-1; y++) {
            for (int x = 0; x < img_dst.cols-1; x++) {
                int v = img_src.data[y * img_src.step + x];
                if (v < thresh) {
                    img_dst.data[y * img_dst.step + x] = 0;
                    error = v;
                } else {
                    img_dst.data[y * img_dst.step + x] = 255;
                    error = v - 255;
                }
            }
        }
        imshow("img_dst", img_dst);


        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
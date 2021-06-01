/**
 * @file random_dithering.cpp
 * @author your name (you@domain.com)
 * @brief ランダムディザリング
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

        /* ランダムディザリング */
        // メルセンヌ・ツイスター法による擬似乱数生成器を、ハードウェア乱数をシードにして初期化
        random_device seed_gen;
        mt19937 engine(seed_gen());
        // 一様分布(0-255)
        uniform_int_distribution<> dist(0, 255);
        for (int y = 0; y < img_dst.rows; y++) {
            for (int x = 0; x < img_dst.cols; x++) {
                int v = img_src.data[y * img_src.step + x];
                if (v < dist(engine))
                    img_dst.data[y * img_dst.step + x] = 0;
                else
                    img_dst.data[y * img_dst.step + x] = 255;
            }
        }
        imshow("img_src", img_src);
        imshow("img_dst", img_dst);


        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
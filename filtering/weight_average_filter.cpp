/**
 * @file weight_average_filter.cpp
 * @author your name (you@domain.com)
 * @brief 加重平均値フィルタ
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

        // 加重平均オペレータによる画像平滑化
        double weighted_op[3][3] = {
            { 1.0/16.0, 2.0/16.0, 1.0/16.0 },
            { 2.0/16.0, 4.0/16.0, 2.0/16.0 },
            { 1.0/16.0, 2.0/16.0, 1.0/16.0 }
        };
        double sum;
        int k, l;
        
        for (int y = 0; y < img_src.rows; y++) {
            for (int x = 0; x < img_src.cols; x++) {
                sum = 0.0;
                for (k = -1; k <= 1; k++) {
                    if (y + k < 0) continue;
                    if (y + k >= img_src.rows) continue;
                    for (l = -1; l <= 1; l++) {
                        if (x + l < 0) continue;
                        if (x + l >= img_src.cols) continue;
                        sum = sum + img_src.data[(y+k) * img_src.step + (x+l)] * weighted_op[k+1][l+1];
                    }
                }
                img_dst.data[y * img_dst.step + x] = sum;
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
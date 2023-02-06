/**
 * @file geo_trans_2d.cpp
 * @author your name (you@domain.com)
 * @brief 回転とアフィン変換
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


float weight_sinc_3poli(float t) {
    /*sinc関数の3多項式で近似した式*/
    float w = 0;
    float f = 0;
    f = std::fabs(t);
    if (f <= 1) {
        w = f*f*f - 2 * f*f + 1;
    }
    else if (f <=2) {
        w = -1 * f*f*f + 5 * f*f + 4;
    }
    else 
        w = 0;

    return w;
}

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
            throw("few parameters.");

        // 画像読み込み
        Mat img_src;
        img_src = imread(argv[1], IMREAD_GRAYSCALE);
        if (img_src.empty())
            throw("failed open file.");

        // 画像準備
        Mat img_dst;
        img_src.copyTo(img_dst);

        ///////////////////////////////////////////
        // 幾何学変換(回転)と最近傍法による再標本化
        ///////////////////////////////////////////
        int src_x, src_y; // 補完によって得られる座標
        float org_x, org_y; // 逆変換によって得られる座標
        float a[2][2]; // 変換行列
        float inv_a[2][2]; // 逆行列
        float det_a; // 行列式

        const float DEG = M_PI / 6; // 30°
        a[0][0] = cos(DEG);
        a[0][1] = -sin(DEG);
        a[1][0] = sin(DEG);
        a[1][1] = cos(DEG);
        det_a = a[0][0] * a[1][1] - a[0][1] * a[1][0];
        inv_a[0][0] = a[1][1] / det_a;
        inv_a[0][1] = -a[0][1] / det_a;
        inv_a[1][0] = -a[1][0] / det_a;
        inv_a[1][1] = a[0][0] / det_a;

        // 途中

        ///////////////////////////////////////////
        // 幾何学変換(回転)と双対一次補完による再標本化
        ///////////////////////////////////////////

        ///////////////////////////////////////////
        // 幾何学変換(回転)と双対三次補完による再標本化
        ///////////////////////////////////////////

        waitKey(0);
    }
    catch (const char *str)
    {
        cerr << str << endl;
    }
    return 0;
}
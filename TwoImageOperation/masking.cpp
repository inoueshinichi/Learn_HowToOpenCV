/**
 * @file masking.cpp
 * @author your name (you@domain.com)
 * @brief マスク合成
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
        Mat img_src1, img_src2;
        img_src1 = img_src_vec[0];
        img_src2 = img_src_vec[1];
        int H = img_src1.rows;
        int W = img_src1.cols;
        Mat img_dst = Mat::zeros(Size(H, W), CV_8UC1);
        Mat img_mskg, img_msk, img_mskn;
        Mat img_s1m, img_s2m;

        // マスク画像生成のための2値化
        img_mskg = threshold(img_src1, img_mskg, 170, 255, THRESH_BINARY_INV);
        imshow("img_mskg", img_mskg);

        // マスク画像（カラー）生成
        //vector<Mat> channels;
        //channels.push_back(img_mskg); // Rのマスク
        //channels.push_back(img_mskg); // Gのマスク
        //channels.push_back(img_mskg); // Bのマスク
        //merge(channels, img_msk); // RGBマスク

        img_src1.copyTo(img_msk);
        cout << "channels() of img_mskg: " << img_mskg.channels() << endl;
        cout << "channels() of img_msk: " << img_msk.channels() << endl;
        for (int y = 0; y < img_msk.rows; y++)
        {
            for (int x = 0; x < img_msk.cols; x++)
            {
                for (int i = 0; i < 3; i++)
                    img_msk.data[y * img_msk.step + x * img_msk.channels() + i] = img_mskg.data[y * img_mskg.step + x];
            }
        }
        imshow("img_msk", img_msk);

        // 入力画像１からマスク画像の部分だけを切り出す（切り出し画像１の生成）: 各画素の3チャンネル（RGB）に対して、bit演算（AND）を行う。 8bit x 3
        // マスク値：255 = 0b11111111, 0 = 0b00000000
        bitwise_and(img_src1, img_msk, img_s1m);

        // マスク画像の反転
        bitwise_not(img_msk, img_mskn);

        // 入力画像２からマスク画像の反転部分だけを切り出す（切り出し画像２の生成）
        bitwise_and(img_src2, img_mskn, img_s2m);

        // 切り出し画像１と切り出し画像２の合成
        bitwise_or(img_s1m, img_s2m, img_dst);

        imshow("img_src1", img_src1);
        imshow("img_src2", img_src2);
        imshow("img_dst", img_dst);

        waitKey(0);
    }
    catch (const char *str)
    {
        cerr << str << endl;
    }
    return 0;
}
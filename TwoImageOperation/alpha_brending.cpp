/**
 * @file multi_image_to_gray_temp.cpp
 * @author your name (you@domain.com)
 * @brief アルファブレンディング
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
        Mat img_dst = Mat::zeros(Size(W, H), CV_8UC1);

        // アルファブレンディング(今回はα=0.5)
        const double alpha = 0.5;
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                auto val1 = img_src1.data[y * img_src1.step + x];
                auto val2 = img_src2.data[y * img_src2.step + x];
                img_dst.data[y * img_dst.step + x] = alpha * val1 + (1 - alpha) * val2;
            }
        }

        // OpenCVによるアルファブレンディング
        // addWeighted(img_src1, alpha, img_src2, (1 - alpha), 0, img_dst);

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
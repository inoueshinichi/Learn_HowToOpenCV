/**
 * @file multi_image_to_gray_temp.cpp
 * @author your name (you@domain.com)
 * @brief 多入力画像の雛形コード
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

int main(int argc, char** argv)
{
    try
    {
        if (argc < 2)
            throw ("few parameters.");

        // 画像読み込み
        vector<Mat> img_src_vec;
        for (int i = 1; i < argc; ++i)
        {
            Mat img_src = imread(argv[i], IMREAD_GRAYSCALE);

            if (img_src.empty())
                throw ("failed open file.");

            img_src_vec.push_back(img_src);
        }
        std::printf("Got %d images\n", img_src_vec.size());

        // 画像準備
        Mat img_dst;
        
        

        // ここに処理を記述


        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
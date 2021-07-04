/**
 * @file dilation_erosion.cpp
 * @author your name (you@domain.com)
 * @brief 膨張、収縮、クロージング、オープニング
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

        // 通常の２値化処理
        Mat img_dst1;
        int thresh = 100;
        threshold(img_src, img_dst1, thresh, 255, THRESH_BINARY);
        imshow("img_dst1", img_dst1);

        // 膨張処理(４近傍)
        Mat element4 = (Mat_<uchar>(3,3) << 0,1,0, 1,1,1, 0,1,0); // 4近傍
        Mat img_dilation;
        img_dst1.copyTo(img_dilation);
        for (int y = 1; y < img_dilation.rows - 1; y++) {
            for (int x = 1; x < img_dilation.cols - 1; x++) {
                if (img_dst1.data[y*img_dst1.step + x] == 255 ||
                    img_dst1.data[(y-1)*img_dst1.step + x] == 255 ||
                    img_dst1.data[y*img_dst1.step + (x-1)] == 255 ||
                    img_dst1.data[y*img_dst1.step + (x+1)] == 255 ||
                    img_dst1.data[(y+1)*img_dst1.step + x] == 255) {
                    
                    img_dilation.data[y*img_dilation.step + x] = 255;
                }
                else {
                    img_dilation.data[y*img_dilation.step + x] = 0;
                }
            }
        }
        //dilate(img_dst1, img_dilation, element4, Point(-1,-1), 1);
        imshow("img_dilation", img_dilation);

        // 収縮処理(４近傍)
        Mat img_erosion;
        img_dst1.copyTo(img_erosion);
        for (int y = 1; y < img_erosion.rows - 1; y++) {
            for (int x = 1; x < img_erosion.cols - 1; x++) {
                if (
                    img_dst1.data[y*img_dst1.step + x] == 0 ||
                    img_dst1.data[(y-1)*img_dst1.step + x] == 0 ||
                    img_dst1.data[y*img_dst1.step + (x-1)] == 0 ||
                    img_dst1.data[y*img_dst1.step + (x+1)] == 0 ||
                    img_dst1.data[(y+1)*img_dst1.step + x] == 0) {
                    
                    img_erosion.data[y*img_erosion.step + x] = 0;
                }
                else {
                    img_erosion.data[y*img_erosion.step + x] = 255;
                }
            }
        }
        //erode(img_dst1, img_erosion, element4, Point(-1,-1), 1);
        imshow("img_erosion", img_erosion);


        // クロージング（n回の膨張処理→n回の収縮処理）: 小さな孔を塞ぎ、分断された連結要素を接続する
        Mat img_crosing;
        Mat element8 = (Mat_<uchar>(3,3) << 1,1,1, 1,1,1, 1,1,1); // 8近傍
        morphologyEx(img_dst1, img_crosing, MORPH_CLOSE, element8, Point(-1,-1), 1);
        imshow("img_crosing", img_crosing);


        // オープニング（n回の収縮処理→n回の膨張処理）: 小さなノイズを取り除く
        Mat img_opening;
        morphologyEx(img_dst1, img_opening, MORPH_OPEN, element8, Point(-1,-1), 1);
        imshow("img_opening", img_opening);
        

        // オープニング・クロージングの組み合わせによるノイズ除去
        // オープニング１回　クロージング１回
        Mat img_tmp, img_Opening1Crosing1;
        morphologyEx(img_dst1, img_tmp, MORPH_OPEN, element8, Point(-1,-1), 1);
        morphologyEx(img_tmp, img_Opening1Crosing1, MORPH_CLOSE, element8, Point(-1,-1), 1);
        imshow("img_Opening1Crosing1", img_Opening1Crosing1);


        // オープニング５回　クロージング５回
        Mat img_tmp2, img_Opening5Crosing5;
        morphologyEx(img_dst1, img_tmp2, MORPH_OPEN, element8, Point(-1,-1), 5);
        morphologyEx(img_tmp2, img_Opening5Crosing5, MORPH_CLOSE, element8, Point(-1,-1), 5);
        imshow("img_Opening5Crosing5", img_Opening5Crosing5);


        // オープニング10回　クロージング10回
        Mat img_tmp3, img_Opening10Crosing10;
        morphologyEx(img_dst1, img_tmp3, MORPH_OPEN, element8, Point(-1,-1), 10);
        morphologyEx(img_tmp3, img_Opening10Crosing10, MORPH_CLOSE, element8, Point(-1,-1), 10);
        imshow("img_Opening10Crosing10", img_Opening10Crosing10);
        
        // オープニング20回　クロージング20回
        Mat img_tmp4, img_Opening20Crosing20;
        morphologyEx(img_dst1, img_tmp4, MORPH_OPEN, element8, Point(-1,-1), 20);
        morphologyEx(img_tmp4, img_Opening20Crosing20, MORPH_CLOSE, element8, Point(-1,-1), 20);
        imshow("img_Opening20Crosing20", img_Opening20Crosing20);

        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
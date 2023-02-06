/**
 * @file perimeter_roundness.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

int main(int argc, char **argv)
{
    try
    {
        // 画像読み込み
        std::string test_file1 = GetTestData("cat_blob.jpg");
        std::string test_file2 = GetTestData("cat.jpg");
        std::cout << "Test file 1 path: " << test_file1 << std::endl;
        std::cout << "Test file 2 path: " << test_file2 << std::endl;

        std::vector<cv::Mat> img_src_vec;
        img_src_vec.reserve(2);
        cv::Mat img_in;
        img_in = cv::imread(test_file1, cv::IMREAD_GRAYSCALE);
        img_src_vec.emplace_back(img_in);
        img_in = cv::imread(test_file2, cv::IMREAD_GRAYSCALE);
        img_src_vec.emplace_back(img_in);
        if (img_src_vec.empty())
            throw("failed open file.");
        std::printf("Got %ld images\n", img_src_vec.size());

        int rows1 = img_src_vec[0].rows;
        int cols1 = img_src_vec[0].cols;
        int rows2 = img_src_vec[1].rows;
        int cols2 = img_src_vec[1].cols;
        if (rows1 != rows2)
        {
            std::printf("rows1: %d, rows2: %d\n", rows1, rows2);
            throw("rows1 != rows2");
        }
        if (cols1 != cols2)
        {
            std::printf("cols1: %d, cols2: %d\n", cols1, cols2);
            throw("cols1 != cols2");
        }

        // 画像準備
        cv::Mat img_fg;               // 前景
        cv::Mat img_bg;               // 背景
        cv::Mat img_diff;             // 差分
        cv::Mat img_bin;              // 2値画像
        cv::Mat img_mask1, img_mask2; // マスク画像

        // 前景/背景 画像
        img_fg = img_src_vec[0].clone();
        img_bg = img_src_vec[1].clone();

        CV_IMSHOW(img_fg)
        CV_IMSHOW(img_bg)

        // 差分画像
        cv::absdiff(img_fg, img_bg, img_diff);
        CV_IMSHOW(img_diff)

        // 差分画像の2値化
        cv::threshold(img_diff, img_bin, 30, 255, cv::THRESH_BINARY);
        CV_IMSHOW(img_bin)

        // 矩形(ブロブ)の抽出
        cv::Rect rect = cv::boundingRect(img_bin);
        float aspect_ratio = (float)rect.height / rect.width;
        std::printf("Aspect ratio of img_bin: %f\n", aspect_ratio); // アスペクト比
        cv::Point top_left = rect.tl();
        cv::Point bottom_right = rect.br();
        cv::Mat img_bin_rect;
        img_bin.copyTo(img_bin_rect);
        cv::cvtColor(img_bin_rect, img_bin_rect, cv::COLOR_GRAY2RGB);
        cv::rectangle(img_bin_rect, top_left, bottom_right, cv::Scalar(0, 255, 0), 1, cv::LINE_8);
        CV_IMSHOW(img_bin_rect)

        // (ブロブ)面積
        // ブロブ　面積(Area)
        uint32_t area = 0;
        for (int y = 0; y < img_bin.rows; y++)
        {
            for (int x = 0; x < img_bin.cols; x++)
            {
                if (img_bin.data[y * img_bin.step + x] == 255)
                    area++;
            }
        }
        std::printf("Area of img_bin: %u\n", area);

        /***** ブロブ　周囲長(Perimiter) *****/
        // 始点の探索
        int ini_x = 0;
        int ini_y = 0;
        bool escape_flag = false;
        for (int y = 0; y < img_bin.rows; y++)
        {
            for (int x = 0; x < img_bin.cols; x++)
            {
                if (img_bin.data[y * img_bin.step + x] == 255)
                {
                    ini_x = x;
                    ini_y = y;
                    escape_flag = true;
                    break;
                }
            }
            if (escape_flag)
            {
                std::cout << "Get first blob !" << std::endl;
                break;
            }
        }

#define ELEMENT4
#if defined (ELEMENT4)
        // ４近傍　上下左右
        const int N = 4;
        int rot_x[N] = {0, 1, 0, -1};
        int rot_y[N] = {1, 0, -1, 0};
#else
        // const int N = 8;
        // int rot_x[N] = { 0, 1, 1, 1, 0, -1, -1, -1 };
        // int rot_y[N] = { 1, 1, 0, -1, -1, -1, 0, 1 };
#endif
        int rot = 0;          // 探索方向
        double perimeter = 0; // 周囲長
        int now_x, now_y;
        int pre_x = ini_x;
        int pre_y = ini_y;

        while (true)
        {
            for (int i = 0; i < N; i++)
            {
                now_x = pre_x + rot_x[(rot + i) % 4];
                now_y = pre_y + rot_y[(rot + i) % 4];
                if (now_x < 0 || now_x > img_bin.cols - 1 || now_y < 0 || now_y > img_bin.rows - 1)
                {
                    continue;
                }
                    
                if (img_bin.data[now_y * img_bin.step + now_x] == 255)
                {
#if defined(ELEMENT4)
                    perimeter++;
#else
                     if (i == 1 || i == 3 || i == 5 || i == 7) {
                         perimeter += sqrt(2); // √2
                     }
                     else {
                         perimeter += 1;
                     }
#endif
                    pre_x = now_x;
                    pre_y = now_y;
                    rot += i + (N - 1); // 次の探索方向は今見つかった方向の１つ前から始める
                    break;
                }
            }
            if (pre_x == ini_x && pre_y == ini_y)
                break;
        }
        std::cout << "Perimeter:" << perimeter << std::endl;

        // ブロブ 円形度(Roundness)
        double roundness = 4 * M_PI * area / perimeter / perimeter;
        std::cout << "Roundness:" << roundness << std::endl;

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
/**
 * @file interpolation.cpp
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

#if _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <cmath>


double w_sinc(double t)
{
    /*sinc関数の3多項式で近似した式*/
    double w = 0;
    double f = 0;
    f = std::fabs(t);
    if (f <= 1)
    {
        w = f * f * f - 2 * f * f + 1;
    }
    else if (f <= 2)
    {
        w = -1 * f * f * f + 5 * f * f - 8 * f + 4;
    }
    else
        w = 0;

    return w;
}

int main(int argc, char **argv)
{
    try
    {
        // テスト画像
        std::string test_file = GetTestData("lena.jpg");
        std::cout << "Test file path: " << test_file << std::endl;

        // 画像読み込み
        cv::Mat img_src;
        img_src = cv::imread(test_file, cv::IMREAD_COLOR);
        if (img_src.empty())
            throw("failed open file.");
        CV_IMSHOW(img_src)

        /* 幾何学変換（回転）と最近傍法による再標本化 */
        int src_ipl_x, src_ipl_y;   // 補完によって得られる座標値（整数）
        float src_inv_x, src_inv_y; // 逆変換によって得られる座標値（実数）
        float a[2][2];      // 変換行列（写像）
        float inv_a[2][2];  // 逆行列
        float det_a;        // 行列式

        // 変換行列=回転行列とする
        const float DEG = M_PI / 6; // 30度
        a[0][0] = cos(DEG);
        a[0][1] = -sin(DEG);
        a[1][0] = sin(DEG);
        a[1][1] = cos(DEG);

        // 回転行列の逆行列
        det_a = a[0][0] * a[1][1] - a[0][1] * a[1][0];
        inv_a[0][0] = a[1][1] / det_a;
        inv_a[0][1] = -a[0][1] / det_a;
        inv_a[1][0] = -a[1][0] / det_a;
        inv_a[1][1] = a[0][0] / det_a;

        // 画像諸元
        int height = img_src.rows;
        int width = img_src.cols;
        int channels = img_src.channels();
        uint32_t mem_per_line = img_src.step;
        
        /*最近傍法による再標本化*/
        cv::Mat img_ipl_nearest;
        img_src.copyTo(img_ipl_nearest);
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                // 逆行列を用いて変換前(img_src)の実数座標を計算
                src_inv_x = inv_a[0][0] * x + inv_a[0][1] * y;
                src_inv_y = inv_a[1][0] * x + inv_a[1][1] * y;

                // 四捨五入で最も近い整数値の座標を計算(最近傍法)
                src_ipl_x = (int)(src_inv_x + 0.5);
                src_ipl_y = (int)(src_inv_y + 0.5);

                for (int c = 0; c < channels; ++c)
                {
                    if ((src_ipl_y >= 0 && src_ipl_y < height) &&
                        (src_ipl_x >= 0 && src_ipl_x < width))
                    {
                        // 変換前の画素値を用いて変換後の画素値を決定(再標本化)
                        img_ipl_nearest.data[y * mem_per_line + x * channels + c] = 
                            img_src.data[src_ipl_y * mem_per_line + src_ipl_x * channels + c];
                    }
                    else
                    {
                        // 範囲外は０埋め
                        img_ipl_nearest.data[y * mem_per_line + x * channels + c] = 0;
                    }
                }
            }
        }
        CV_IMSHOW(img_ipl_nearest)


        /* 双1次補完法による再標本化 */
        cv::Mat img_ipl_bilinear;
        img_src.copyTo(img_ipl_bilinear);
        int src_bl_x1, src_bl_x2, src_bl_y1, src_bl_y2; // 補間で使う周囲4点
        float src_bl_d1, src_bl_d2, src_bl_d3, src_bl_d4; // 周囲4点に対応する重み(距離)
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                for (int c = 0; c < channels; ++c)
                {
                    // 逆行列を用いて変換前(img_src)の実数座標を計算
                    src_inv_x = inv_a[0][0] * x + inv_a[0][1] * y;
                    src_inv_y = inv_a[1][0] * x + inv_a[1][1] * y;

                    // 双1次補完法で必要になる周辺4点を求める
                    src_bl_x1 = (int)(src_inv_x);
                    src_bl_x2 = src_bl_x1 + 1;
                    src_bl_y1 = (int)(src_inv_y);
                    src_bl_y2 = src_bl_y1 + 1;

                    // (src_inv_x, src_inv_y)から周囲4点までの距離(重み)
                    src_bl_d1 = src_inv_x - src_bl_x1;
                    src_bl_d2 = src_bl_x2 - src_inv_x;
                    src_bl_d3 = src_inv_y - src_bl_y1;
                    src_bl_d4 = src_bl_y2 - src_inv_y;

                
                    if ((src_inv_y >= 0 && src_inv_y < height) &&
                        (src_inv_x >= 0 && src_inv_x < width))
                    {
                        // 周辺4点の座標を使って画素値の加重平均を求める
                        uchar pxl_x1y1 = img_src.data[src_bl_y1 * mem_per_line + src_bl_x1 * channels + c];
                        uchar pxl_x1y2 = img_src.data[src_bl_y2 * mem_per_line + src_bl_x1 * channels + c];
                        uchar pxl_x2y1 = img_src.data[src_bl_y1 * mem_per_line + src_bl_x2 * channels + c];
                        uchar pxl_x2y2 = img_src.data[src_bl_y2 * mem_per_line + src_bl_x2 * channels + c];

                        // d1*d4*I([x]+1,[y]) + d1*d3*I([x]+1,[y]+1) + d2*d4*I([x],[y]) + d2*d3*I([x],[y]+1)
                        img_ipl_bilinear.data[y * mem_per_line + x * channels + c] = cv::saturate_cast<uchar>(
                            src_bl_d1*src_bl_d4*pxl_x2y1 + 
                            src_bl_d1*src_bl_d3*pxl_x2y2 + 
                            src_bl_d2*src_bl_d4*pxl_x1y1 +
                            src_bl_d2*src_bl_d3*pxl_x1y2);
                    }
                    else
                    {
                        // 範囲外は０埋め
                        img_ipl_bilinear.data[y * mem_per_line + x * channels + c] = 0;
                    }
                }
            }
        }
        CV_IMSHOW(img_ipl_bilinear)


        /* 双3次補完法による再標本化 */
        cv::Mat img_ipl_bicubic;
        img_src.copyTo(img_ipl_bicubic);
        int src_bc_x[4], src_bc_y[4];   // 周囲16点の座標値
        double src_bc_dx[4], src_bc_dy[4]; // 周囲16点までの距離
        double src_bc_wx[4], src_bc_wy[4]; // 周囲16点までの重み

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                for (int c = 0; c < channels; ++c)
                {
                    // 逆行列を用いて変換前(img_src)の実数座標を計算
                    src_inv_x = inv_a[0][0] * x + inv_a[0][1] * y;
                    src_inv_y = inv_a[1][0] * x + inv_a[1][1] * y;

                    if (src_inv_x < 1 || src_inv_x > (width - 2) ||
                        src_inv_y < 1 || src_inv_y > (height - 2))
                    {
                        // 範囲外はゼロ埋め
                        img_ipl_bicubic.data[y * mem_per_line + x * channels + c] = 0;
                        continue;
                    }

                    // 双3次補完法で必要になる周辺16点のx座標y座標を求める
                    src_bc_x[0] = int(src_inv_x) - 1;
                    src_bc_x[1] = int(src_inv_x);
                    src_bc_x[2] = int(src_inv_x) + 1;
                    src_bc_x[3] = int(src_inv_x) + 2;
                    src_bc_y[0] = int(src_inv_y) - 1;
                    src_bc_y[1] = int(src_inv_y);
                    src_bc_y[2] = int(src_inv_y) + 1;
                    src_bc_y[3] = int(src_inv_y) + 2;

                    // 双3次補完法で必要になる周辺16点の画素値
                    uchar f11 = img_src.data[src_bc_y[0] * mem_per_line + src_bc_x[0] * channels + c];
                    uchar f12 = img_src.data[src_bc_y[0] * mem_per_line + src_bc_x[1] * channels + c];
                    uchar f13 = img_src.data[src_bc_y[0] * mem_per_line + src_bc_x[2] * channels + c];
                    uchar f14 = img_src.data[src_bc_y[0] * mem_per_line + src_bc_x[3] * channels + c];
                    uchar f21 = img_src.data[src_bc_y[1] * mem_per_line + src_bc_x[0] * channels + c];
                    uchar f22 = img_src.data[src_bc_y[1] * mem_per_line + src_bc_x[1] * channels + c];
                    uchar f23 = img_src.data[src_bc_y[1] * mem_per_line + src_bc_x[2] * channels + c];
                    uchar f24 = img_src.data[src_bc_y[1] * mem_per_line + src_bc_x[3] * channels + c];
                    uchar f31 = img_src.data[src_bc_y[2] * mem_per_line + src_bc_x[0] * channels + c];
                    uchar f32 = img_src.data[src_bc_y[2] * mem_per_line + src_bc_x[1] * channels + c];
                    uchar f33 = img_src.data[src_bc_y[2] * mem_per_line + src_bc_x[2] * channels + c];
                    uchar f34 = img_src.data[src_bc_y[2] * mem_per_line + src_bc_x[3] * channels + c];
                    uchar f41 = img_src.data[src_bc_y[3] * mem_per_line + src_bc_x[0] * channels + c];
                    uchar f42 = img_src.data[src_bc_y[3] * mem_per_line + src_bc_x[1] * channels + c];
                    uchar f43 = img_src.data[src_bc_y[3] * mem_per_line + src_bc_x[2] * channels + c];
                    uchar f44 = img_src.data[src_bc_y[3] * mem_per_line + src_bc_x[3] * channels + c];

                    // 双3次補完法で必要になる周囲16点までの距離
                    src_bc_dx[0] = src_inv_x - src_bc_x[0];
                    src_bc_dx[1] = src_inv_x - src_bc_x[1];
                    src_bc_dx[2] = src_bc_x[2] - src_inv_x;
                    src_bc_dx[3] = src_bc_x[3] - src_inv_x;
                    src_bc_dy[0] = src_inv_y - src_bc_y[0];
                    src_bc_dy[1] = src_inv_y - src_bc_y[1];
                    src_bc_dy[2] = src_bc_y[2] - src_inv_y;
                    src_bc_dy[3] = src_bc_y[3] - src_inv_y;

                    // 双3次補間法で必要になる周囲16点までの重み(sinc関数)
                    src_bc_wx[0] = w_sinc(src_bc_dx[0]);
                    src_bc_wx[1] = w_sinc(src_bc_dx[1]);
                    src_bc_wx[2] = w_sinc(src_bc_dx[2]);
                    src_bc_wx[3] = w_sinc(src_bc_dx[3]);
                    src_bc_wy[0] = w_sinc(src_bc_dy[0]);
                    src_bc_wy[1] = w_sinc(src_bc_dy[1]);
                    src_bc_wy[2] = w_sinc(src_bc_dy[2]);
                    src_bc_wy[3] = w_sinc(src_bc_dy[3]);

                    // 途中計算
                    double v1 = f11 * src_bc_wy[0] + f12 * src_bc_wy[1] + f13 * src_bc_wy[2] + f14 * src_bc_wy[3];
                    double v2 = f21 * src_bc_wy[0] + f22 * src_bc_wy[1] + f23 * src_bc_wy[2] + f24 * src_bc_wy[3];
                    double v3 = f31 * src_bc_wy[0] + f32 * src_bc_wy[1] + f33 * src_bc_wy[2] + f34 * src_bc_wy[3];
                    double v4 = f41 * src_bc_wy[0] + f42 * src_bc_wy[1] + f43 * src_bc_wy[2] + f44 * src_bc_wy[3];

                    img_ipl_bicubic.data[y * mem_per_line + x * channels + c] = cv::saturate_cast<uchar>(
                        v1 * src_bc_wx[0] + v2 * src_bc_wx[1] + v3 * src_bc_wx[2] + v4 * src_bc_wx[3]
                    );
                }
            }
        }
        CV_IMSHOW(img_ipl_bicubic)

        cv::waitKey(0);
    }
    catch (const char **str)
    {
        std::cerr << str << std::endl;
    }

    return 0;
}
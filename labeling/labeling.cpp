/**
 * @file labeling.cpp
 * @author your name (you@domain.com)
 * @brief ラベリング
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

        // 2値化
        Mat img_binary;
        double thresh = 200;
        threshold(img_src, img_binary, thresh, 255, THRESH_BINARY_INV);
        imshow("img_binary", img_binary);

        /*OpenCVを使わない場合の簡易ラベリング*/
        int nlabel = 0;
        int w = img_binary.cols;
        int h = img_binary.rows;
        const int TABLESIZE = 1024; // 1回目の走査で蓄えるラベルの最大数
        static int table[TABLESIZE]; // ラベル対応表
        table[0] = 0; // [0, 1, 2, 3, 4, ..., n] max:n -> 1024

        // 1回目の走査
        // 結果はimg_labelに入れる
        Mat img_label = Mat::zeros(Size(h, w), CV_8U);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++w) {
                int lum = img_binary.data[y*img_binary.step + x];

                // 注目画素が黒の場合
                if (lum == 0) {
                    img_label.data[y*img_label.step + x] = 0;
                }
                else {
                    // 注目画素が白の場合

                    /* 上3画素と左1画素をチェック
                        (3x3)
                        123
                        4@■
                        ■■■
                    */
                    const int N = 4;
                    const int dx[N] = {-1, 0, 1, -1};
                    const int dy[N] = {-1, -1, -1, 0};
                    vector<int> list(4, 0);
                    int count = 0;

                    // 注目画素の該当する4画素が白の場合、カウントを1進める
                    for (int k = 0; k < N; ++k) {
                        int xdx = x + dx[k];
                        int ydy = y + dy[k];
                        int val = img_label.data[ydy * img_label.step + xdx];
                        if (xdx >= 0 &&
                            ydy >= 0 &&
                            xdx < w  &&
                            ydy < h) {
                                if (val != 0) {
                                    list[count] = table[val];
                                    count++;
                                }
                            }
                    }

                    // 該当する4画素にいずれもラベル(>0)がなかった場合、
                    // 新規のラベルを作成
                    if (count == 0) {
                        nlabel++;
                        img_label.data[y * img_label.step + x] = nlabel;
                        table[nlabel] = nlabel;
                    } else {
                        // 注目画素周辺の4画素の前回のラベル状態を更新

                        // listを昇順ソート　1,2,4,3 -> 1,2,3,4
                        sort(list.begin(), list.end()); 

                        // listから重複を除いたものをlist_uniqueへ格納
                        vector<int> list_unique;
                        int uniq = 1;
                        list_unique.push_back(list[0]);
                        for (int k = 1; k < count; ++k) {
                            if (list[k] != list_unique[uniq - 1]) {
                                list_unique.push_back(list[k]);
                                uniq++;
                            }
                        }

                        /**
                         * @brief 上下で隣接してないが、斜めで隣接しているブロックを同じラベルに書き換える
                         * 
                         * 123
                         * 4@■
                         * ■■■
                         * 
                         * 00000000000000      00000000000000
                         * 00001110000000      00001110000000
                         * 00011110000000      00011110000000
                         * 0000000□□□□□00  ->  00000001□□□□00
                         * 0000000□□□□□00      0000000□□□□□00
                         * 0000000□□□□000      0000000□□□□000
                         * 00000000000000      00000000000000
                         */

                        // 注目画素にlist_uniqueの最小ラベルを挿入
                        img_label.data[y*img_label.step + x] = list_unique[0];

                        // テーブルの更新
                        for (int k = 1; k < uniq; ++k) {
                            table[list_unique[k]] = list_unique[k]; // ここの処理がよくわからない
                        }

                        // 値のクリア&メモリ解放
                        list_unique.clear();
                        vector<int>().swap(list_unique);
                    }

                    // 値のクリア＆メモリ解放
                    list.clear();
                    vector<int>().swap(list);
                }
            }   
        }

        // 隣接しているラベルを統合し、ラベル値の中抜けがないように変換表を作る
        static unsigned char table2[TABLESIZE];
        int k2 = 0;
        const int MAXVALUE = 255;
        for (int k = 0; k <= nlabel; ++k) {
            if (table[k] == k) {
                // ラベル値の対応が一致している場合、中抜けのない新たな対応表に追加
                if (k2 <= MAXVALUE) {
                    table2[k] = k2;
                    k2++;
                } else {
                    // MAXVALUEより大きなラベルは捨てる
                    table2[k] = 0;
                }
            } else {
                // ラベル値の対応が一致していない場合：対応するラベル値を探す
                int kk = k;
                do {
                    kk = table[kk];
                } while (table[kk] != kk);
                table[k] = kk;
            }
        }

        // 2回目の走査
        Mat img_dst = Mat::zeros(Size(h,w), CV_8U);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                img_dst.data[y*img_dst.step + x] = table2[table[img_label.data[y*w+x]]];
            }
        }
        imshow("img_dst", img_dst);


        // /*OpenCVによるラベリング(かなり高性能)*/
        // Mat img_dst2, img_lbl;
        // int nlabel_ = connectedComponents(img_binary, img_lbl);
        // compare(img_lbl, 7, img_dst2, CMP_EQ); // ラベル7を抜き出す
        // std::cout << "nlabel:" << nlabel_ << std::endl;
        // imshow("img_dst2", img_dst2);
        // imshow("img_lbl", img_lbl);


        /*ラベルブロブの形状特徴*/
        // 本来はラベル毎に抽出して行うが、ここでは、img_binaryに簡易的に1つのブロブが存在することを想定

        // ブロブ、アスペクト比(縦/横)
        Rect rect = boundingRect(img_binary);
        double aspect_ratio = (double)(rect.height / rect.width);
        std::cout << "aspect_ratio: " << aspect_ratio << std::endl;
        Mat img_bounding_rect;
        img_binary.copyTo(img_bounding_rect);
        Point tl = rect.tl();
        Point br = rect.br();
        rectangle(img_bounding_rect, tl, br, Scalar(255,0,0), 1, LINE_8);
        imshow("img_bounding_rect", img_bounding_rect);

        // ブロブの面積
        int area = 0;
        for (int y=0; y<img_binary.rows; ++y) {
            for (int x=0; x<img_binary.cols; ++x) {
                if (img_binary.data[y*img_binary.step + x] == 255) {
                    area++;
                }
            }
        }
        std::cout << "area: " << area << std::endl;

        // ブロブの周囲長(Perimeter)
        // 始点の探索
        int init_x = 0;
        int init_y = 0;
        bool escape_flag = false;
        for (int y=0; y<img_binary.rows; ++y) {
            for (int x=0; x<img_binary.cols; ++x) {
                if (img_binary.data[y*img_binary.step+x] == 255) {
                    init_x = x;
                    init_y = y;
                    escape_flag = true;
                    break;
                }
            }
            if (escape_flag) {
                std::cout << "first point got " << std::endl;
                break;
            }
        }
        // ４近傍　上下左右
        const int N = 4;
        int rot_x[N] = {0, 1, 0, -1};
        int rot_y[N] = {1, 0, -1, 0};
        //const int N = 8;
        //int rot_x[N] = { 0, 1, 1, 1, 0, -1, -1, -1 };
        //int rot_y[N] = { 1, 1, 0, -1, -1, -1, 0, 1 };
        int rot = 0;          // 探索方向
        double perimeter = 0; // 周囲長
        int now_x, now_y;
        int pre_x = init_x;
        int pre_y = init_y;
        while (true) {
            for (int index = 0; index < N; ++index) { // 4近傍
                now_x = pre_x + rot_x[(rot + index) % N];
                now_y = pre_y + rot_y[(rot + index) % N];

                if (now_x < 0 ||
                    now_x >= img_binary.cols ||
                    now_y < 0 ||
                    now_y >= img_binary.rows) {
                    continue;
                }

                int lum = img_binary.data[now_y*img_binary.step + now_x];
                if (lum == 255) 
                {
                    perimeter++;

                    /* 8近傍ver
                    if (index == 1 ||
                        index == 3 || 
                        index == 5 || 
                        index == 7) {
                        perimeter += sqrt(2); // 斜めは√2を追加する
                    }
                    else {
                        perimeter += 1;
                    }
                    */
                    pre_x = now_x;
                    pre_y = now_y;
                    rot += index + (N - index); // 次の探索方向は今回見つかった方向の1つ前
                    break;
                }

                // 終了条件
                if (pre_x == init_x && pre_y == init_y)
                    break;
            }
        }
        std::cout << "Perimeter: " << perimeter << std::endl;


        /*ブロブの円形度(Roundness)の計算*/
        double roundness = 4 * M_PI * area / perimeter / perimeter;
        std::cout << "Roundness: " << roundness << std::endl;

        /*重心と主軸角度*/
        int count = 0;
        double x_g = 0.0;
        double y_g = 0.0;
        double x_d = 0.0;
        double y_d = 0.0;
        double xy_d = 0.0;
        for (int y = 0; img_binary.rows; ++y) {
            for (int x = 0; img_binary.cols; ++x) {
                int lum = img_binary.data[y*img_binary.step+x];
                if (lum == 255)  {
                    count++;
                    x_g += x;
                    y_g += y;
                }
            }
        }
        x_g /= count;
        y_g /= count;
        // 慣性主軸の角度
        for (int y = 0; y < img_binary.rows; ++y) {
            for (int x = 0; x < img_binary.cols; ++x) {
                x_d += (x - x_g) * (x - x_g);
                y_d += (y - y_g) * (y - y_g);
                xy_d += (x - x_g) * (y - y_g);
            }
        }
        double angle = 0.5 * atan2(2 * xy_d, x_d - y_d) / M_PI : 180.0;
        std::cout << "Angle: " << angle << std::endl;

        // // OpenCV 面積・周囲長・円形度
        // vector<vector<Point> > contours; // 輪郭
        // findContours(img_binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        // area = contourArea(Mat(contours[0]));          // 面積
        // perimeter = arcLength(Mat(contours[0]), true); // 周囲長
        // roundness = 4 * M_PI * area / perimeter / perimeter;
        // cout << "OpenCV Area:" << area << endl;
        // cout << "Perimeter:" << perimeter << endl;
        // cout << "OpenCV Roundness:" << roundness << endl;

        // // OpenCV 重心　慣性主軸の角度
        // Moments m = moments(img_binary, true);
        // // 面積
        // area = m.m00;
        // cout << "area: " << area << endl;
        // // 重心
        // x_g = m.m10 / m.m00;
        // y_g = m.m01 / m.m00;
        // cout << x_g << " " << y_g << endl;
        // // 主軸の角度
        // ang = 0.5 * atan2(2.0 * m.mu11, m.mu20 - m.mu02);
        // cout << ang * 180 / M_PI << endl;

        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
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
                    vector<int>.swap(list);
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


        /*OpenCVによるラベリング(かなり高性能)*/
        Mat img_dst2, img_lbl;
        int nlabel_ = connectedComponents(img_binary, img_lbl);
        compare(img_lbl, 7, img_dst2, CMP_EQ); // ラベル7を抜き出す
        std::cout << "nlabel:" << nlabel_ << std::endl;
        imshow("img_dst2", img_dst2);
        imshow("img_lbl", img_lbl);

        waitKey(0);
    }
    catch (const char* str)
    {
        cerr << str << endl;
    }
    return 0;
}
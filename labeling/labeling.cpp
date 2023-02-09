/**
 * @file labeling.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-09
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
        std::string test_file1 = GetTestData("catmod.jpg");
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

        // オープニング(ノイズ除去)
        cv::Mat element8 = (cv::Mat_<uchar>(3, 3)
                                << 1,
                            1, 1, 1, 1, 1, 1, 1, 1); // 8近傍

        // ノイズ除去 & 埋めあわせ
        cv::Mat img_bin_clear;
        int n = 2;
        cv::morphologyEx(img_bin, img_bin_clear, cv::MORPH_OPEN, element8, cv::Point(-1, -1), n);
        cv::morphologyEx(img_bin_clear, img_bin_clear, cv::MORPH_CLOSE, element8, cv::Point(-1, -1), n);
        CV_IMSHOW(img_bin_clear)

// #define MY_LABELING
#if defined(MY_LABELING)
    // 動かない

        /*OpenCVを使わない場合の簡易ラベリング*/
        // 4近傍連結2回走査アルゴリズム
        // Step1. 左上画像から走査し, 白画素を探索する
        // Step2. 注目画素の4近傍(上3画素と左1画素)の画素値を調べ, すべて黒の場合, 注目画素に新しいラベルをつける. 
        //        白画素があり, それらがすべて同じラベルであれば注目画素にもそのラベルをつける. 同じラベルでない場合
        //        最小のラベル値を注目画素につけて, これらのラベルが同じ連結成分であることを記憶しておく
        // Step3. 最後まで走査が終わったら, すべての画素を走査して連結成分を同一ラベル化する
        int numLabel = 0;
        int width = img_bin_clear.cols;
        int height = img_bin_clear.rows;
        const unsigned int tableSize = 1024; // 1回目の操作で蓄える最大ラベル数(多分, ラベル数x4)
        unsigned int labelTable[tableSize]; // ラベル対応表
        labelTable[0] = 0; // [0, 1, 2, 3, 4, ..., Max]
        cv::Mat img_tmp_label = cv::Mat::zeros(cv::Size(height, width), CV_32SC1);
        const int N = 4;
        const int dx[N] = {-1, 0, 1, -1};
        const int dy[N] = {-1, -1, -1, 0};

        // 1回目の走査
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                uchar lum = img_bin_clear.data[y * img_bin_clear.step + x];

                // 注目画素が黒の場合
                if (lum == 0)
                {
                    img_tmp_label.data[y * img_tmp_label.step + x] = 0;
                }
                else
                {
                    /* 上3画素と左1画素をチェック -> 4画素=labelList
                        (3x3)
                        123
                        4@■
                        ■■■
                    */
                    std::array<int, N> labelList = {0, 0, 0, 0};
                    int count = 0;

                    // i) 注目画素の周囲4画素が白の場合, カウントを1つ進める
                    for (int k = 0; k < N; ++k)
                    {
                        int xdx = x + dx[k];
                        int ydy = y + dy[k];
                        int label = img_tmp_label.data[ydy * img_tmp_label.step + xdx];
                        if (xdx >= 0 && 
                            ydy >= 0 && 
                            xdx < width && 
                            ydy < height && 
                            label != 0)
                        {
                            labelList[count] = labelTable[label];
                            count++;
                        }
                    }

                    if (count == 0)
                    {
                        // ii) 周囲4画素にいずれもラベル(>0)がなかった場合, 新規ラベル作成
                        if (numLabel < tableSize - 1)
                        {
                            numLabel++;
                            img_tmp_label.data[y * img_tmp_label.step + x] = numLabel;
                            labelTable[numLabel] = numLabel;
                        }
                    }
                    else
                    {
                        // iii) 周辺の4画素の前回のラベル状態を更新

                        // 周囲4画素のラベルをソート 1,2,4,3 -> 1,2,3,4
                        std::sort(labelList.begin() , labelList.end());

                        // リストから重複を除いたもの
                        std::array<int, N> uniqueLabelList = {0, 0, 0, 0};
                        int uniq = 1;
                        uniqueLabelList[0] = labelList[0];
                        for (int k = 1; k < count; ++k)
                        {
                            if (labelList[1] != uniqueLabelList[uniq - 1])
                            {
                                uniqueLabelList[uniq] = labelList[k];
                                uniq++;
                            }
                        }
                        

                        /**
                         * @brief iv) 上下で隣接してないが、斜めで隣接しているブロックを同じラベルに書き換える
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
                        // 注目画素にuniqueLabelListの最小ラベルを挿入
                        img_tmp_label.data[y * img_tmp_label.step + x] = uniqueLabelList[0];

                        // ラベル値が2以上の場合, テーブルの更新
                        for (int j = 1; j < uniq; ++j)
                        {
                            labelTable[uniqueLabelList[j]] = uniqueLabelList[0];
                        }
                    }
                }
            }
        }

        // 連結しているラベルを統合し、ラベル値の中抜けがないように変換表を作る
        unsigned int resultLabelTable[tableSize];
        unsigned int value = 0;
        const unsigned int maxValue = 255;
        for (int t = 0; t <= numLabel; ++t)
        {
            if (labelTable[t] == t)
            {
                // ラベル値の対応が一致している場合、中抜けのない新たな対応表に追加
                if (value <= maxValue)
                {
                    resultLabelTable[t] = value;
                    value++;
                }
                else
                {
                    // maxValueより大きなラベルは捨てる
                    resultLabelTable[t] = 0;
                }
            }
            else
            {
                // ラベル値の対応が一致していない場合：対応するラベル値を探す
                unsigned int tt = t;
                do {
                    tt = labelTable[tt];
                } while (labelTable[tt] != tt);
                labelTable[t] = tt;
            }
        }

        // 2回目の走査
        cv::Mat img_label = cv::Mat::zeros(cv::Size(height, width), CV_32SC1);
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                img_label.data[y * img_label.step + x] = 
                resultLabelTable[
                    labelTable[
                        img_tmp_label.data[y * img_tmp_label.step + x]
                    ]
                ];
            }
        }

#else
        /*OpenCVによるラベリング(かなり高性能)*/
        cv::Mat img_label;
        int numLabel = cv::connectedComponents(img_bin_clear, img_label);

#endif

        std::cout << "Num of label : " << numLabel << std::endl;
        CV_TYPE_LOG(img_label)

        // 表示用ラベル
        cv::Mat img_show_label;
        img_label.convertTo(img_show_label, CV_8UC1);
        img_show_label *= 30; // 各要素を30倍して分布がわかりやすいようにする
        CV_TYPE_LOG(img_show_label)
        CV_IMSHOW(img_show_label)

        // 指定ラベルの抽出
        int target_label = 3;
        cv::Mat img_target_mask;
        cv::compare(img_label, cv::Scalar(target_label), img_target_mask, cv::CMP_EQ);
        /*
            CMP_EQ ==
            CMP_GT >
            CMP_GE >=
            CMP_LT <
            CMP_LT <=
            CMP_NE !=
        */
        CV_TYPE_LOG(img_target_mask)

        cv::Mat img_bin_target_blob; // 抽出画像
        img_bin.copyTo(img_bin_target_blob, img_target_mask);
        CV_IMSHOW(img_bin_target_blob)


        /*GeometricFeaturesを計算する*/
        std::cout << "###### Taret Blob Geometric Features ######" << std::endl;
        
        // アスペクト比
        cv::Rect rect = cv::boundingRect(img_bin_target_blob);
        float aspect_ratio = (float)rect.height / rect.width;
        std::cout << "Aspect ratio: " <<  aspect_ratio << std::endl;

        // 面積・重心・主軸角度
        cv::Moments m = cv::moments(img_bin_target_blob, true);

        double area_by_moment = m.m00;
        std::cout << "Area_by_moment: " << area_by_moment << std::endl;

        double x_g = m.m10 / m.m00;
        double y_g = m.m01 / m.m00;
        std::printf("COG: (%d,%d)\n", (int)x_g, (int)x_g);

        double angle = 0.5 * std::atan2(2.0 * m.mu11, m.mu20 - m.mu02);
        std::cout << "Angle: " << angle << std::endl;

        // 面積・周囲長・円形度
        std::vector<std::vector<cv::Point>> contours; // 輪郭
        cv::findContours(img_bin_target_blob, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
        double area_by_contour;
        double perimeter;
        double roundness;
        area_by_contour = cv::contourArea(cv::Mat(contours[0]));
        perimeter = cv::arcLength(cv::Mat(contours[0]), true);
        roundness = 4 * M_PI / perimeter / perimeter;
        std::printf("Area_by_contour %f, Perimeter %f, Roundness %f\n", area_by_contour, perimeter, roundness);

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
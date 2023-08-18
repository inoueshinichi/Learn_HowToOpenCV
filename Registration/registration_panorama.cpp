/**
 * @file panorama.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief パノラマ写真作成
 * @version 0.1
 * @date 2023-08-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>
#include <../FileSystem/filesystem/glob.hpp> // 自作

// スルー画像の取り込み処理
CV_DEFINE_PROCESSFRAME

/**
 * @brief パノラマ写真の作成
 * 5枚の連続写真をつなぎ合わせる
 * 
 * 方法
 * 1. 各写真における局所特徴量とキーポイントの抽出 (A->B->C->D->E) ※計算コストの小さいORGやAKAZEを使用する.
 * 2. 各写真間で局所特徴量とキーポイントのマッチングを行う(Bluee-Force(総当り) or FLANN(Fast library for approximate nearest neighbers)[k最近傍法の近似手法])
 * 3. A->Bの座標変換(射影変換H)を作成(B座標をA座標に変換) ※RANSACでアウトライアーを除外する
 * 4. 縦横2倍のサイズの画像サイズを準備する
 * 5. 中心を基準に, 一つ前(A座標系に変換したB座標をAに張り合わせる)
 * 6. 重ね合わせた部分は, アルファブレンディング等で合成する
 */

auto main(int, char**) -> int
{
    try 
    {
        // 入力画像の取り込み
        std::string pattern = GetTestData("panorama/*.JPG");
        auto pathList = is::common::glob_paths(pattern);
        std::ostringstream oss;
        oss << "[PathList]\n";
        for (const auto &path : pathList)
        {
            oss << path << std::endl;
        }
        std::cout << oss.str();

        int pictureSize = pathList.size();
        std::cout << "Picture size: " << pictureSize << std::endl;

        std::vector<cv::Mat> imgList;
        for (const auto &path : pathList)
        {
            imgList.push_back(cv::imread(path, cv::IMREAD_COLOR)); // RGBで読み込み
        }

        // 画像サイズ
        int height = 0;
        int width = 0;

        // もし入力画像の画像サイズが異なれば, エラー出力
        if (pictureSize > 0)
        {
            height = imgList[0].cols;
            width = imgList[0].rows;
            for (int i = 1; i < pictureSize; i++)
            {
                if (height != imgList[i].cols || 
                    width != imgList[i].rows) 
                {
                    throw std::runtime_error("All size of images must be same.");
                }
            }
            std::cout << "[OK] all image size are same." << std::endl;
        }

        int srcType = imgList[0].type();
        std::string typeStr = cv::typeToString(srcType);
        std::cout << "[OK] image type is " << typeStr << std::endl;
        

        // グレースケールに変換
        std::vector<cv::Mat> imgGreyList;
        for (const auto &img : imgList)
        {
            cv::Mat img_gray;
            cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY); // BGR -> Gray
            imgGreyList.push_back(img_gray);
        }

        // 出力画像 (入力画像サイズの縦横2倍サイズ)
        cv::Mat img_dst = cv::Mat::zeros(cv::Size(2 * width, 2 * height), srcType);



        /* パノラマ画像の作成 */

        // AKAZE
        /** @brief The AKAZE constructor

            @param descriptor_type Type of the extracted descriptor: DESCRIPTOR_KAZE,
            DESCRIPTOR_KAZE_UPRIGHT, DESCRIPTOR_MLDB or DESCRIPTOR_MLDB_UPRIGHT.
            @param descriptor_size Size of the descriptor in bits. 0 -\> Full size
            @param descriptor_channels Number of channels in the descriptor (1, 2, 3)
            @param threshold Detector response threshold to accept point
            @param nOctaves Maximum octave evolution of the image
            @param nOctaveLayers Default number of sublevels per scale level
            @param diffusivity Diffusivity type. DIFF_PM_G1, DIFF_PM_G2, DIFF_WEICKERT or
            DIFF_CHARBONNIER
        */

        // AKAZEアルゴリズム
        cv::Ptr</*cv::Feature2D*/cv::AKAZE> detector = cv::AKAZE::create();
        std::vector<cv::KeyPoint> keyPointsPrev, keyPointsNext;
        cv::Mat descriptionPrev, descriptionNext;

        // マッチングアルゴリズム
        cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
        int top_k = 20;
        float match_judge = 0.6f;

        // 射影行列
        cv::Mat H;

        // 色
        cv::Scalar colors[2] = { 
            cv::Scalar(0, 0, 255), // 赤
            cv::Scalar(255, 0, 0)  // 青
        };

        for (int i = 0; i < pictureSize - 1; ++i)
        {
            // 初期化
            keyPointsPrev.clear();
            keyPointsNext.clear();

            // キーポイントと局所特徴量の計算
            detector->detectAndCompute(imgGreyList[i], cv::noArray(), keyPointsPrev, descriptionPrev);
            detector->detectAndCompute(imgGreyList[i+1], cv::noArray(), keyPointsNext, descriptionNext);


        }
        

    } 
    catch (char *str)
    {
        std::cout << *str << std::endl;
    }
    return 0;
}


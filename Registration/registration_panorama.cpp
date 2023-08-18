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

// #define FLLAN_MATCHING


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

        // 画像の型をチェック
        int srcType = imgList[0].type();
        std::string typeStr = cv::typeToString(srcType);
        std::cout << "[OK] image type is " << typeStr << std::endl;

        // 色
        cv::RNG random;
        std::vector<cv::Scalar> colors;
        for (int i = 0; i < pictureSize; i++)
        {
            // ランダムに色を生成
            cv::Scalar color = cv::Scalar(random.uniform(0, 255), 
                                          random.uniform(0, 255), 
                                          random.uniform(0, 255));
            colors.push_back(color);
        }

        // 射影変換後の画像を張り付けるスペース
        std::vector<cv::Mat> imgBgSpaceList;

        // 入力画像の処理
        std::vector<cv::Mat> imgGreyList;
        cv::Mat img_gray;
        cv::Mat img_bg;
        cv::Mat img_src;
        for (int i = 0; i < pictureSize; i++)
        {
            // グレースケールに変換
            cv::cvtColor(imgList[i], img_gray, cv::COLOR_BGR2GRAY); // BGR -> Gray
            imgGreyList.push_back(img_gray);

            // 背景の作成(黒)
            // (入力画像サイズの縦横2倍サイズ)
            img_bg = cv::Mat::zeros(cv::Size(2 * width, 2 * height), srcType);
            imgBgSpaceList.push_back(img_bg);

            // 外枠の描画
            cv::rectangle(img_src, cv::Point(0,0), cv::Point(width, height), colors[i]);

            // 縦横2倍の背景の中央に画像を張り付ける
            img_src = img_gray.clone();
            cv::Mat roi = img_bg(cv::Rect(width/4, height/4, width, height));
            img_src.copyTo(roi); // Offset(width/4,height/4)を基準に(width,height)の領域に張り付ける.
        }

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
        std::vector<cv::KeyPoint> keyPointsPrev, keyPointsNext; // キーポイント
        cv::Mat descriptionPrev, descriptionNext; // 局所特徴量

        // マッチングアルゴリズム
        // enum MatcherType
        // {
        //     FLANNBASED = 1,
        //     BRUTEFORCE = 2,
        //     BRUTEFORCE_L1 = 3,
        //     BRUTEFORCE_HAMMING = 4,
        //     BRUTEFORCE_HAMMINGLUT = 5,
        //     BRUTEFORCE_SL2 = 6
        // };

        // cv::DMatch
        // distance: 特徴量記述子の距離
        // queryIdx: クエリ記述子(match(desc1, desc2) と渡した場合、desc1 のインデックス)
        // trainIdx: 学習記述子(match(desc1, desc2) と渡した場合、desc2 のインデックス)
#if !defined(FLLAN_MATCHING)

        cv::Ptr<cv::DescriptorMatcher> matcher =
            cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE_HAMMING);
        int best = 20;
        std::vector<cv::DMatch> matches; // n at unit pair images

#else
        cv::Ptr<cv::DescriptorMatcher> matcher =
            cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
        int top_k = 2;
        float match_judge = 0.6f;

        // Read below hint.
        std::vector<std::vector<cv::DMatch>> matches;
        /* n x m = Nの特徴点ペアの内, k個抜き取る?
            img1_kp[0] : match[0,0], match[0,1], ..., match[0,top_k]
            img1_kp[1] : match[1,0], match[1,2], ..., match[1,top_k]
                    :
                    :
            img1_kp[n] : match[k,0], match[k,2], ..., match[k,top_k]
        */

        descriptionPrev.convertTo(descriptionPrev, CV_32F);
        descriptionNext.convertTo(descriptionNext, CV_32F);
#endif

        // 射影行列
        cv::Mat H;

//         for (int i = 0; i < pictureSize - 1; ++i)
//         {
//             // 初期化
//             keyPointsPrev.clear();
//             keyPointsNext.clear();

//             // キーポイントと局所特徴量の計算
//             detector->detectAndCompute(imgGreyList[i], cv::noArray(), keyPointsPrev, descriptionPrev);
//             detector->detectAndCompute(imgGreyList[i+1], cv::noArray(), keyPointsNext, descriptionNext);

//             std::printf("[%d]th img keypoints: %d >>>| |<<< [%d]th img keypoints: %d\n",
//                         i, (int)keyPointsPrev.size(), i + i, (int)keyPointsNext.size());

//             // 必要キーポイント数に満たない場合, エラーを返す
//             if (keyPointsPrev.size() < best ||
//                 keyPointsNext.size() < best)
//             {
//                 throw std::runtime_error("Few keypoints against best num points");
//             }

//             /* 特徴点の対応付け */

//             // 特徴点対応の初期化
//             matches.clear();

// #if !defined(FLLAN_MATCHING)

//             // Bluet-Force(総当り) N = n * m
//             // (query, trained)
//             matcher->match(descriptionPrev,
//                            descriptionNext,
//                            matches);

//             // 上位best個を採用
//             // 基準となる要素よりも小さい要素が前に来るよう並べ替える
//             // https://cpprefjp.github.io/reference/algorithm/nth_element.html
//             // 下記は, best番目(20)よりも小さいdistanceスコアを先頭側に詰めるように並び替える.
//             std::nth_element(matches.begin(), matches.begin() + /*基準値を指定する部分*/ best - 1, matches.end()); // for distance

//             // best以下(つまり, 上位bestよりも距離が離れているpair)を削除
//             matches.erase(matches.begin() + best, matches.end());

//             std::cout << "best = " << best << std::endl;
// #else
//             // 上位k位までのマッチング(query, trained) N = n * tok_k
//             matcher->knnMatch(descriptionPrev, descriptionNext, matches, top_k);

//             // 対応点を絞る
//             std::vector<cv::DMatch> goodMatches;
//             std::vector<cv::Point2f> goodPoints1;
//             std::vector<cv::Point2f> goodPoints2;

//             for (int i = 0; i < matches.size(); ++i)
//             {
//                 // 比較対象の2枚の画像について, 片方の特徴点1つに対してもう片方で2点が対応している(top_k = 2)
//                 float dist1 = matches[i][0].distance;
//                 float dist2 = matches[i][1].distance;

//                 // 良い点を残す（最も類似する点と次に類似する点の類似度から）
//                 if (dist1 <= dist2 * match_judge)
//                 {
//                     goodMatches.emplace_back(matches[i][0]);

//                     int curIndex = matches[i][0].queryIdx;
//                     int lastIndex = matches[i][0].trainIdx;
//                     goodPoints1.emplace_back(keyPoints.at(curIndex).pt);
//                     goodPoints2.emplace_back(lastKeyPoints.at(lastIndex).pt);
//                 }
//             }

//             std::cout << "top_k = " << top_k << std::endl;
// #endif
//             std::cout << "match size = " << matches.size() << std::endl;

//             // 特徴点対応を描画

            
//         }

        // 表示用
        cv::Mat img_bin_rgb;
        cv::Mat img_raw_resized;
        cv::Mat img_bg_resized;
        cv::Mat img_match_resized;
        cv::Mat img_dst_resized;

        // 表示画像
        int unit_width = (int)(width / pictureSize);
        int unit_height = (int)(height / pictureSize);
        cv::Mat img_show = cv::Mat::zeros(cv::Size(width, height), srcType);

        // 表示用画像に入力画像を張り付ける
        for (int i = 0; i < pictureSize; i++)
        {
            // 縮小
            img_src = imgList[i].clone();
            cv::resize(img_src, img_raw_resized, cv::Size(unit_width, unit_height));

            // 部分領域に書き込む
            int left = i * unit_width;
            int top = 0;
            // int right = left + unit_width;
            // int bottom = top + unit_height;
            cv::Mat roi = img_show(cv::Rect(left, top, unit_width, unit_height));
            img_raw_resized.copyTo(roi);
        }

        unit_width = (int)(2 * width / pictureSize);
        unit_height = (int)(2 * height / pictureSize);
        for (int i = 0; i < pictureSize; i++)
        {
            cv::resize(imgBgSpaceList.at(i), img_bg_resized, cv::Size(unit_width, unit_height));
            int left = i * unit_width;
            int top = unit_height;
            cv::Mat roi = img_show(cv::Rect(left, top, unit_width, unit_height));
            img_bg_resized.copyTo(roi);
        }

        std::cout << "[Done]" << std::endl;

        CV_IMSHOW(img_show)
        cv::waitKey(0); // 待機
    } 
    catch (char *str)
    {
        std::cout << *str << std::endl;
    }
    return 0;
}


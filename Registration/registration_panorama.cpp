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

#define FLLAN_MATCHING
// #define DEBUG_LOG

auto main(int, char **) -> int
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
            height = imgList[0].rows;
            width = imgList[0].cols;
            for (int i = 1; i < pictureSize; i++)
            {
                if (height != imgList[i].rows ||
                    width != imgList[i].cols)
                {
                    throw std::runtime_error("All size of images must be same.");
                }
            }
            std::cout << "[OK] all image size are same." << std::endl;
        }

        int unit_width = (int)(width / pictureSize);
        int unit_height = (int)(height / pictureSize);

        // 画像の型をチェック
        int srcType = imgList[0].type();
        std::string typeStr = cv::typeToString(srcType);
        std::cout << "[OK] image type is " << typeStr << std::endl;

        // 色
        cv::RNG random;
        std::vector<cv::Scalar> colors;
        // for (int i = 0; i < pictureSize; i++)
        // {
        //     // ランダムに色を生成
        //     cv::Scalar color = cv::Scalar(random.uniform(0, 255),
        //                                   random.uniform(0, 255),
        //                                   random.uniform(0, 255));
        //     colors.push_back(color);
        // }
        colors.push_back(cv::Scalar(255, 0, 0));   // 0: 青
        colors.push_back(cv::Scalar(0, 255, 0));   // 1: 緑
        colors.push_back(cv::Scalar(0, 0, 255));   // 2: 赤
        colors.push_back(cv::Scalar(0, 255, 255)); // 3: 黄
        colors.push_back(cv::Scalar(255, 255, 0)); // 4: シアン

        // パノラマ画像
        cv::Mat img_panorama = cv::Mat::zeros(cv::Size(2 * width, 2 * height), srcType);

        // グレースケール
        std::vector<cv::Mat> imgGreyList(pictureSize);

        // 射影変換後の画像を張り付けるスペース
        std::vector<cv::Mat> imgBgSpaceList(pictureSize);

        // (100x100)の正方形を射影行列したものを張り付けるスペース
        std::vector<cv::Mat> imgPerspectiveSpaceList(pictureSize);
        for (int i = 0; i < pictureSize; i++)
        {
            // 初期化
            cv::Mat init = cv::Mat::zeros(imgList[i].size(), imgList[i].type());
            init.copyTo(imgPerspectiveSpaceList[i]);
        }

        for (int i = 0; i < pictureSize; i++)
        {
            // グレースケールに変換
            cv::Mat img_grey;
            cv::cvtColor(imgList[i], img_grey, cv::COLOR_BGR2GRAY); // BGR -> Gray
            img_grey.copyTo(imgGreyList[i]);

#if defined(DEBUG_LOG)
            if (i == 0)
            {
                // グレースケールはOK
                CV_IMSHOW(img_grey)
                cv::waitKey(0); // 待機
            }
#endif

            // 背景の作成(黒)
            // (入力画像サイズの縦横2倍サイズ)
            cv::Mat bgSpace = cv::Mat::zeros(cv::Size(2 * width, 2 * height), srcType);
            imgBgSpaceList[i] = bgSpace;

#if defined(DEBUG_LOG)
            if (i == 0)
            {
                // 背景
                CV_IMSHOW(bgSpace)
                cv::waitKey(0); // 待機
            }
#endif

            // 縦横2倍の背景の中央に画像を張り付ける
            cv::Mat img_grey_rgb;
            cv::cvtColor(imgGreyList[i], img_grey_rgb, cv::COLOR_GRAY2BGR); // gray_rgb

            // こっちでもOK GrayScale(1 channel) -> RGB(3 channels)
            // std::vector<cv::Mat> monoVec;
            // monoVec.push_back(imgGreyList[i]);
            // monoVec.push_back(imgGreyList[i]);
            // monoVec.push_back(imgGreyList[i]);
            // cv::merge(monoVec, img_grey_rgb);

            // 外枠の描画
            cv::rectangle(img_grey_rgb, cv::Point(0, 0), cv::Point(width, height), colors[i]);

            // 部分領域の貼り付け
            cv::Mat roi = imgBgSpaceList[i](cv::Rect(imgBgSpaceList[i].cols / 4, imgBgSpaceList[i].rows / 4, width, height));
            img_grey_rgb.copyTo(roi); // Offset(width/4,height/4)を基準に(width,height)の領域に張り付ける.

            if (i == 0)
            {
                roi = img_panorama(cv::Rect(imgBgSpaceList[i].cols / 4, imgBgSpaceList[i].rows / 4, width, height));
                img_grey_rgb.copyTo(roi); // 一番最初(左端)画像を張り付ける.
            }

            // dummy OK
            // cv::Mat scalar = cv::Mat(cv::Size(width, height), srcType, cv::Scalar(0, 255, 0));
            // scalar.copyTo(roi);
        }

        std::cout << "[OK] process input images" << std::endl;

        /* パノラマ画像の作成 */

        // 表示画像
        cv::Mat img_show = cv::Mat::zeros(cv::Size(width, height), srcType);

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
        cv::Ptr</*cv::Feature2D*/ cv::AKAZE> detector = cv::AKAZE::create();
        std::vector<cv::KeyPoint> keyPointsPrev, keyPointsNext; // キーポイント
        cv::Mat descriptionPrev, descriptionNext;               // 局所特徴量

        /**
         * @brief cv::KeyPoint
         * public:
         *     cv::Point2f pt; // pos
         *     float size;  // diameter
         *     float angle; // orientation
         *     float response; // length of keypoint
         *     int octave; // pyramid octave
         *     int class_id; // object id
         */

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
#endif

        // 有効特徴点
        std::vector<cv::KeyPoint> prevValidKeyPoints, nextValidKeyPoints;

        // 射影行列
        std::vector<cv::Mat> HArray;

        for (int i = 1; i < pictureSize; ++i)
        {
            // 初期化
            keyPointsPrev.clear();
            keyPointsNext.clear();

            // キーポイントと局所特徴量の計算
            detector->detectAndCompute(imgGreyList[i - 1], cv::noArray(), keyPointsPrev, descriptionPrev);
            detector->detectAndCompute(imgGreyList[i], cv::noArray(), keyPointsNext, descriptionNext);

#if defined(FLLAN_MATCHING)
            // floating points
            descriptionPrev.convertTo(descriptionPrev, CV_32F);
            descriptionNext.convertTo(descriptionNext, CV_32F);
#endif

            std::printf("[%d]th img keypoints: %d >>>| |<<< [%d]th img keypoints: %d\n",
                        i - 1, (int)keyPointsPrev.size(), i, (int)keyPointsNext.size());

            /* 特徴点の対応付け */
            std::cout << "Matching process ..." << std::endl;

            // 特徴点対応の初期化
            matches.clear();

#if !defined(FLLAN_MATCHING)

            // 必要キーポイント数に満たない場合, エラーを返す
            if (keyPointsPrev.size() < best ||
                keyPointsNext.size() < best)
            {
                throw std::runtime_error("Few keypoints against best num points");
            }

            // Bluet-Force(総当り) N = n * m
            std::printf("Bluet-Force(総当り)を実行 (時間かかる)\n");

            // (query, trained)
            matcher->match(descriptionPrev,
                           descriptionNext,
                           matches);

            // 上位best個を採用
            // 基準となる要素よりも小さい要素が前に来るよう並べ替える
            // https://cpprefjp.github.io/reference/algorithm/nth_element.html
            // 下記は, best番目(20)よりも小さいdistanceスコアを先頭側に詰めるように並び替える.
            std::nth_element(matches.begin(), matches.begin() + /*基準値を指定する部分*/ best - 1, matches.end()); // for distance

            // best以下(つまり, 上位bestよりも距離が離れているpair)を削除
            matches.erase(matches.begin() + best, matches.end());

            std::cout << "best = " << best << std::endl;

            // 有効特徴点の初期化
            prevValidKeyPoints.clear();
            nextValidKeyPoints.clear();
            for (int idx = 0; idx < matches.size(); idx++)
            {
                prevValidKeyPoints.push_back(keyPointsPrev[matches[idx].queryIdx]);
                nextValidKeyPoints.push_back(keyPointsNext[matches[idx].trainIdx]);

                // prevValidKeyPoints.push_back(keyPointsPrev[matches[idx].trainIdx]);
                // nextValidKeyPoints.push_back(keyPointsNext[matches[idx].queryIdx]);
            }
#else
            // 上位k位までのマッチング(query, trained) N = n * tok_k
            matcher->knnMatch(descriptionPrev, descriptionNext, matches, top_k);

            // 良好な対応点に絞る
            std::vector<cv::DMatch> goodMatches;
            std::vector<cv::KeyPoint> goodKeyPointsPrev;
            std::vector<cv::KeyPoint> goodKeyPointsNext;

            for (int idx = 0; idx < matches.size(); ++idx)
            {
                // 比較対象の2枚の画像について, 片方の特徴点1つに対してもう片方で2点が対応している(top_k = 2)
                float dist1 = matches[idx][0].distance;
                float dist2 = matches[idx][1].distance;

                // 良い点を残す（最も類似する点と次に類似する点の類似度から）
                if (dist1 <= dist2 * match_judge)
                {
                    goodMatches.emplace_back(matches[idx][0]);

                    // インデックス
                    int prevIndex = matches[idx][0].queryIdx;
                    int nextIndex = matches[idx][0].trainIdx;
                    // int nextIndex = matches[idx][0].queryIdx;
                    // int prevIndex = matches[idx][0].trainIdx;

                    // cv::KeyPoint
                    goodKeyPointsPrev.emplace_back(keyPointsPrev.at(prevIndex));
                    goodKeyPointsNext.emplace_back(keyPointsNext.at(nextIndex));
                }
            }

            std::printf("[%d]th (prev) good kp: %d <<< | | >>> [%d]th (next) good kp: %d\n",
                        i - 1, (int)goodKeyPointsPrev.size(), i, (int)goodKeyPointsNext.size());

            std::printf("Good matches by fllan: %d\n", (int)goodMatches.size());

            std::cout << "top_k = " << top_k << std::endl;

            // 有効特徴点の初期化
            prevValidKeyPoints.clear();
            nextValidKeyPoints.clear();

            // 有効特徴点を取得
            std::copy(goodKeyPointsPrev.begin(), goodKeyPointsPrev.end(), std::back_inserter(prevValidKeyPoints));
            std::copy(goodKeyPointsNext.begin(), goodKeyPointsNext.end(), std::back_inserter(nextValidKeyPoints));
#endif
            std::cout << "match size = " << matches.size() << std::endl;

            /* ここから共通処理 */

            // ホモグラフィ行列Hを求める
            // Step1. RANSACでインライアの特徴点対応を取得.
            // Step2. インライアのみを用いて射影行列Hを求める.
            if (prevValidKeyPoints.size() < 4 || nextValidKeyPoints.size() < 4) // 対応点が4組以上ある場合に計算可能
            {
                std::printf("Stop on [%d]th view. Because keypoints needed for caclulating homography matrix is less than 4.\n", i);
                std::printf("prevValidKeyPoints.size() : %d, nextValidKeyPoints.size() : %d\n",
                            (int)prevValidKeyPoints.size(), (int)nextValidKeyPoints.size());
                break;
            }
            cv::Mat H;     // 射影行列
            cv::Mat maskH; // インライアマスク, 1 : インライア, 0 : アウトライア

            // RANSACを用いた透視行列(H)の計算
            double ransacReprojThreshold = 3.0;
            std::vector<cv::Point2f> validPointsPrev, validPointsNext;
            cv::KeyPoint::convert(prevValidKeyPoints, validPointsPrev); // KeyPoint -> Point2f
            cv::KeyPoint::convert(nextValidKeyPoints, validPointsNext); // KeyPoint -> Point2f
            H = cv::findHomography(validPointsNext,
                                   validPointsPrev,
                                   maskH,
                                   cv::RANSAC,
                                   ransacReprojThreshold);

            CV_TYPE_LOG(maskH);                                               // CV_8UC1
            std::printf("Shape of maskH: (%d,%d)\n", maskH.rows, maskH.cols); // (N,1)

            std::cout << "H = " << cv::format(H, cv::Formatter::FMT_NUMPY) << std::endl;

            // RANSACで得られたインライアーのみを抽出
            std::cout << "Getting only inliers..." << std::endl;
            std::vector<cv::DMatch> inlierMatches;
            std::vector<cv::KeyPoint> inlierKeyPointsPrev;
            std::vector<cv::KeyPoint> inlierKeyPointsNext;
            for (int idx = 0; idx < maskH.rows; ++idx)
            {
                uchar *inliner = maskH.ptr<uchar>(idx);
                if (inliner[0] == 1)
                {
                    inlierMatches.emplace_back(goodMatches[idx]);
                    inlierKeyPointsPrev.emplace_back(goodKeyPointsPrev[idx]);
                    inlierKeyPointsNext.emplace_back(goodKeyPointsNext[idx]);

                    // if (inlierMatches.size() == 4)
                    // {
                    //     break; // 4点のみ使う
                    // }
                }
            }

            // インライアーな対応点のみを用いて再度ホモグラフィ行列Hを計算
            std::vector<cv::Point2f> inlierPointsPrev, inlierPointsNext;
            cv::KeyPoint::convert(inlierKeyPointsPrev, inlierPointsPrev); // KeyPoint -> Point2f
            cv::KeyPoint::convert(inlierKeyPointsNext, inlierPointsNext); // KeyPoint -> Point2f
            H = cv::findHomography(inlierPointsNext,
                                   inlierPointsPrev,
                                   maskH,
                                   cv::RANSAC,
                                   ransacReprojThreshold);

            std::cout << "inlier H = " << cv::format(H, cv::Formatter::FMT_NUMPY) << std::endl;

            // 4点のみの場合
            // H = cv::getPerspectiveTransform(inlierPoints1, inlierPoints2, cv::DECOMP_LU);
            /* 第3引数 solver
                DECOMP_LU (default)
                DECOMP_SVD
                DECOMP_EIG
                DECOMP_CHOLESKY
                DECOMP_QR
                DECOMP_NORMAL
            */

            if (H.empty())
            {
                std::cout << "[Warning] H is empty!" << std::endl;
                continue;
            }

            // 射影変換
            cv::Mat img_transformed;
            cv::warpPerspective(imgBgSpaceList[i], img_transformed, H, imgBgSpaceList[i].size());

            // 画像の重ね合わせ
            cv::addWeighted(img_transformed, /*alpha*/ 0.5, imgBgSpaceList[i], /*beta*/ 0.5, /*gamma*/ 0, img_panorama);

            // 特徴点対応を描画
            cv::Mat img_match;

            // 特徴点位置をオフセット

            // インライア集合
            std::vector<cv::Point2f> offsetInlierPointsPrev;
            std::vector<cv::Point2f> offsetInlierPointsNext;
            std::transform(inlierPointsPrev.begin(),
                           inlierPointsPrev.end(),
                           std::back_inserter(offsetInlierPointsPrev),
                           [&](const cv::Point2f &pt) -> cv::Point2f
                           {
                               cv::Point2f offsetPoint = cv::Point2f(pt.x + imgBgSpaceList[i].cols / 4,
                                                                     pt.y + imgBgSpaceList[i].rows / 4);
                               return offsetPoint;
                           });
            std::transform(inlierPointsNext.begin(),
                           inlierPointsNext.end(),
                           std::back_inserter(offsetInlierPointsNext),
                           [&](const cv::Point2f &pt) -> cv::Point2f
                           {
                               cv::Point2f offsetPoint = cv::Point2f(pt.x + imgBgSpaceList[i].cols / 4,
                                                                     pt.y + imgBgSpaceList[i].rows / 4);
                               return offsetPoint;
                           });

            // 全集合
            std::vector<cv::KeyPoint> offsetKeyPointsPrev;
            std::vector<cv::KeyPoint> offsetKeyPointsNext;
            std::transform(keyPointsPrev.begin(),
                           keyPointsPrev.end(),
                           std::back_inserter(offsetKeyPointsPrev),
                           [&](const cv::KeyPoint &kp) -> cv::KeyPoint
                           {
                               cv::KeyPoint keypoint(/*pt*/ kp.pt + cv::Point2f(imgBgSpaceList[i].cols / 4, imgBgSpaceList[i].rows / 4),
                                                     /*size*/ kp.size,
                                                     /*angle*/ kp.angle,
                                                     /*response*/ kp.response,
                                                     /*octave*/ kp.octave,
                                                     /*class_id*/ kp.class_id);
                               return keypoint;
                           });
            std::transform(keyPointsNext.begin(),
                           keyPointsNext.end(),
                           std::back_inserter(offsetKeyPointsNext),
                           [&](const cv::KeyPoint &kp) -> cv::KeyPoint
                           {
                               cv::KeyPoint keypoint(/*pt*/ kp.pt + cv::Point2f(imgBgSpaceList[i].cols / 4, imgBgSpaceList[i].rows / 4),
                                                     /*size*/ kp.size,
                                                     /*angle*/ kp.angle,
                                                     /*response*/ kp.response,
                                                     /*octave*/ kp.octave,
                                                     /*class_id*/ kp.class_id);
                               return keypoint;
                           });

            std::cout << "keyPointsPrev size: " << keyPointsPrev.size() << std::endl;
            std::cout << "keyPointsNext size: " << keyPointsNext.size() << std::endl;

            // std::cout << "inlierMatches size: " << inlierMatches.size() << std::endl;
            // for (int n = 0; n < inlierMatches.size(); n++)
            // {
            //     std::printf("[%d]th match: queryIdx -> %d, trainIdx -> %d\n", n, inlierMatches[n].queryIdx, inlierMatches[n].trainIdx);
            // }

            // マッチングチェック OK
            // cv::drawMatches(imgBgSpaceList[i],
            //                 offsetKeyPointsPrev,
            //                 imgBgSpaceList[i+1],
            //                 offsetKeyPointsNext,
            //                 inlierMatches,
            //                 img_match);
            // CV_IMSHOW(img_match)
            // cv::waitKey(0); // 待機

            // 前フレーム面に対して現フレーム面の傾き度合いを四角形□(100x100)を射影変換したもので表現
            std::vector<cv::Point2f> rectPoints;
            rectPoints.reserve(4);
            rectPoints.emplace_back(cv::Point2f(0.0f, 0.0f));     // 左上
            rectPoints.emplace_back(cv::Point2f(100.0f, 0.0f));   // 右上
            rectPoints.emplace_back(cv::Point2f(100.0f, 100.0f)); // 右下
            rectPoints.emplace_back(cv::Point2f(0.0f, 100.0f));   // 右下

            std::vector<cv::Point2f> rectHPoints;
            rectHPoints.reserve(4);
            cv::perspectiveTransform(rectPoints, rectHPoints, H); // 点の透視変換
            for (int k = 0; k < 4; k++)
            {
                std::cout << "rectHPoints[" << k << "]: (" << rectHPoints[k].x << "," << rectHPoints[k].y << ")" << std::endl;
            }

            // 描画用に平行移動
            std::for_each(rectHPoints.begin(),
                          rectHPoints.end(),
                          [&](cv::Point2f &pt)
                          {
                              pt.x += unit_width / 4 + (i - 1) * unit_width;
                              pt.y += height / 2 - unit_height / 4;
                          });

            // 描画
            cv::line(img_show, (cv::Point)(rectHPoints[0]), (cv::Point)(rectHPoints[1]), colors[i - 1] /*cv::Scalar(255, 255, 255)*/, /*thickness*/ 3);
            cv::line(img_show, (cv::Point)(rectHPoints[1]), (cv::Point)(rectHPoints[2]), colors[i - 1] /*cv::Scalar(255, 255, 255)*/, /*thickness*/ 3);
            cv::line(img_show, (cv::Point)(rectHPoints[2]), (cv::Point)(rectHPoints[3]), colors[i - 1] /*cv::Scalar(255, 255, 255)*/, /*thickness*/ 3);
            cv::line(img_show, (cv::Point)(rectHPoints[3]), (cv::Point)(rectHPoints[0]), colors[i - 1] /*cv::Scalar(255, 255, 255)*/, /*thickness*/ 3);

            cv::circle(img_show, /*center*/ cv::Point(width / 2, height / 2), /*radius*/ 20, /*color*/ cv::Scalar(255, 255, 255), /*thickness*/ -1);
        } // for int i = 1

        // 表示用
        cv::Mat img_bin_rgb;
        cv::Mat img_raw_resized;
        cv::Mat img_bg_resized;
        cv::Mat img_perspective_resized;
        cv::Mat img_match_resized;
        cv::Mat img_dst_resized;

        // 表示用画像に入力画像を張り付ける
        for (int i = 0; i < pictureSize; i++)
        {
            // 縮小
            cv::resize(imgList[i], img_raw_resized, cv::Size(unit_width, unit_height));

            // 部分領域に書き込む
            int left = i * unit_width;
            int top = 0;
            // int right = left + unit_width;
            // int bottom = top + unit_height;
            cv::Mat roi = img_show(cv::Rect(left, top, unit_width, unit_height));
            img_raw_resized.copyTo(roi);
        }

        // 表示画像に縦横2倍サイズで中央にグレースケールがある画像を張り付ける
        for (int i = 0; i < pictureSize; i++)
        {
            cv::resize(imgBgSpaceList[i], img_bg_resized, cv::Size(unit_width, unit_height));
            int left = i * unit_width;
            int top = unit_height;
            cv::Mat roi = img_show(cv::Rect(left, top, unit_width, unit_height));
            img_bg_resized.copyTo(roi);
        }

        // // 表示画像に(100x100)正方形の射影変換後の四角形画像を張り付ける
        // for (int i = 0; i < pictureSize; i++)
        // {
        //     // cv::Mat white(imgList[i].size(), imgList[i].type(), cv::Scalar(255,255,255));

        //     cv::resize(/*white*/imgPerspectiveSpaceList[i], img_perspective_resized, cv::Size(unit_width, unit_height));
        //     int left = i * unit_width;
        //     int top = 2 * unit_height;
        //     cv::Mat roi = img_show(cv::Rect(left, top, unit_width, unit_height));
        //     img_perspective_resized.copyTo(roi);
        // }

        // 線分
        cv::line(img_show,
                 cv::Point(0, 2 * unit_height),
                 cv::Point(img_show.cols, 2 * unit_height),
                 /*color*/ cv::Scalar(255, 255, 255),
                 /*thickness*/ 1);

        // 線分
        cv::line(img_show,
                 cv::Point(0, 3 * unit_height),
                 cv::Point(img_show.cols, 3 * unit_height),
                 /*color*/ cv::Scalar(255, 255, 255),
                 /*thickness*/ 1);

        CV_IMSHOW(img_panorama)
        cv::waitKey(0); // 待機

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

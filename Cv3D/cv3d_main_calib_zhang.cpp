/**
 * @file cv3d_main_calib_zhang.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief キャリブレーション
 * @version 0.1
 * @date 2023-04-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include <cv3d_def.hpp>
#include <cv3d_zhang_calib.hpp>

#include <filesystem.hpp>

// #define CIRCLE_GRID
constexpr float GRID_SPAN = 10.0; // [mm]

// グリッド
struct Grid
{
    static std::string sCalibPatternName;
    static std::pair<int, int> sGridSize; // (rows, cols)
    static float sGridSpan;

    int mGridLength;
    std::vector<std::pair<int, int>> mGridIndices;
    SpacePoints mGridSpacePoints;
    ImagePoints mGridImagePoints;

    cv::Mat mImg;
    std::string mCalibImgPath;
    bool mSuccessFindCorners;

    ~Grid() {}

    Grid() : mSuccessFindCorners(false)
    {
        int rows = Rows();
        int cols = Cols();
        mGridLength = rows * cols;

        // メモリ確保 for ImagePoints
        mGridImagePoints.reserve(mGridLength);
        mGridImagePoints.resize(mGridLength);

        // Indices, SpacePointsの生成
        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                // Indice
                mGridIndices.push_back({row, cols});

                // SpacePoint
                SpacePoint sp; // [x,y,z] = [*,*,0]
                sp.x = sGridSpan * (row - int(row / 2));
                sp.y = -1.0 * (sGridSpan * (col - int(col / 2))); // パターン中心より上側を`+`とする.
                sp.z = 0.0f;
                mGridSpacePoints.emplace_back(sp);
            }
        }
    }

    static int Rows() { return sGridSize.first; }
    static int Cols() { return sGridSize.second; }
    int Length() const { return mGridLength; }

    ImagePoint &GetImagePoint(int row, int col)
    {
        assert(row >= 0);
        assert(row < Rows());
        assert(col >= 0);
        assert(col < Cols());
        return mGridImagePoints.at(row * Rows() + col);
    }

    SpacePoint &GetSpacePoint(int row, int col)
    {
        assert(row >= 0);
        assert(row < Rows());
        assert(col >= 0);
        assert(col < Cols());
        return mGridSpacePoints.at(row * Rows() + col);
    }
};

float Grid::sGridSpan = GRID_SPAN;

// キャリブレーションパターンのグリッド数
#if !defined(CIRCLE_GRID)
std::string Grid::sCalibPatternName = "SquareGridPattern";
std::pair<int, int> Grid::sGridSize = {3, 5};
cv::TermCriteria termCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 0.001);
#else
std::string Grid::sCalibPatternName = "CircleGridPattern";
std::pair<int, int> Grid::sGridSize = {5, 7};
#endif

auto main(int, char **) -> int
{
    try
    {
        /* code */
        std::cout << "Start camera calibration..." << std::endl;

        /*キャリブレーション準備*/
        std::string calibPatternDir = GetTestData("Calib3DPatterns");    // Src
        std::string resultCalibPatternDir = calibPatternDir + "/Result"; // Dst
        if (!is::common::isexist(resultCalibPatternDir))
        {
            is::common::mkdir(resultCalibPatternDir);
        }
        std::string srcImgDir = calibPatternDir + "/" + Grid::sCalibPatternName;
        auto imgPathList = is::common::glob_paths(srcImgDir + "/*.bmp");
        std::cout << "[Pattern paths]: \n";
        for (auto path : imgPathList)
        {
            std::cout << path << std::endl;
        }
        int batchSize = imgPathList.size();
        std::vector<Grid> mBatchGrids;
        for (int batchIndex = 0; batchIndex < batchSize; ++batchIndex)
        {
            Grid grid;
            grid.mCalibImgPath = imgPathList[batchIndex];
            grid.mImg = cv::imread(imgPathList[batchIndex], cv::IMREAD_COLOR);
            mBatchGrids.emplace_back(grid);
        }

        /*対応点(2Dと3D)の作成*/
        std::vector<std::pair<ImagePoints, ImagePoints>> batchCompare2DPoints;
        for (int batchIndex = 0; batchIndex < batchSize; ++batchIndex)
        {
            Grid &grid = mBatchGrids[batchIndex];

            // パターンの有効性チェック
            if (!cv::checkChessboard(grid.mImg, grid.mImg.size()))
            {
                std::printf("[%dth] Invalid checkboard. %s\n", batchIndex, grid.mCalibImgPath.c_str());
                // throw std::runtime_error("Invalid checkboard.");
                continue;
            }

            // パターンの2D点を検出
            bool bRet;
            int num2DPoints = 0;
            ImagePoints points, subPixPoints;
#if !defined(CIRCLE_GRID)
            bRet = cv::findChessboardCorners(grid.mImg,
                                             cv::Size(Grid::Cols(), Grid::Rows()), // @warning
                                             points,                               // corner
                                             cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
            // サブピクセル推定
            std::copy(points.begin(), points.end(), subPixPoints.begin());
            if (bRet)
            {
                // https://docs.opencv.org/4.7.0/dd/d1a/group__imgproc__feature.html#ga354e0d7c86d0d9da75de9b9701a9a87e
                cv::cornerSubPix(grid.mImg,
                                 subPixPoints,     // corner
                                 cv::Size(11, 11), // Half of search window size.
                                 cv::Size(-1, -1),
                                 termCriteria);
            }
#else
            bRet = cv::findCirclesGrid(grid.mImg,
                                       cv::Size(Grid::Cols(), Grid::Rows()), // @warning
                                       points,                               // center
                                       cv::CALIB_CB_SYMMETRIC_GRID);

            subPixPoints = points;
#endif

            num2DPoints = points.size();
            batchCompare2DPoints.emplace_back(std::make_pair(points, subPixPoints));

            std::ostringstream oss;
            oss << "[" << batchIndex << "th] View ----------" << std::endl;
            for (int i = 0; i < num2DPoints; ++i)
            {
                oss << "[" << i << "th] Pt(" << points[i].x << "," << points[i].y << ") SubPixPt("
                    << subPixPoints[i].x << "," << subPixPoints[i].y << ")" << std::endl;
            }
            std::cout << oss.str() << std::endl;

            if (bRet)
            {
                assert(grid.mGridImagePoints.size() == subPixPoints.size());
                grid.mGridImagePoints = subPixPoints;
            }
            grid.mSuccessFindCorners = bRet;

        } // for

        /*2D点検出したキャリブレーションパターンの表示*/
        if (mBatchGrids.size() > 0)
        {

#define SHOW_INDEX 0

#if defined(SHOW_INDEX)
            cv::Mat detected2DPtImg;
            cv::drawChessboardCorners(
                detected2DPtImg,
                cv::Size(Grid::Cols(), Grid::Rows()), // @warning
                mBatchGrids.at(SHOW_INDEX).mGridImagePoints,
                mBatchGrids.at(SHOW_INDEX).mSuccessFindCorners);

            CV_IMSHOW(detected2DPtImg);
            cv::waitKey(0);
#else

#endif
        }
        /*キャリブレーション実行*/

        /*キャリブレーション結果の確認*/
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
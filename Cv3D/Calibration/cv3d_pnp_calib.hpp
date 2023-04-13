/**
 * @file cv3d_pnp_calib.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-13
 *
 * @note https://docs.opencv.org/4.7.0/d9/d0c/group__calib3d.html#ga549c2075fac14829ff4a58bc931c033d
 * @note https://docs.opencv.org/4.7.0/d5/d1f/calib3d_solvePnP.html#calib3d_solvePnP_flags
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <opencv2/calib3d.hpp>    // calibrateCamera
#include <opencv2/core/types.hpp> // TermCriteria

#include <cv3d_def.hpp>

#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>

/*キャリブレーション with cv::solvePnP*/
void CalibPnP(
    const BatchSpacePoints &batchSpacePoints,
    const BatchImagePoints &batchImagePoints,
    const ImageShape &imageShape,
    const CamIntrinsicParams &initCamInParams,
    const LenDistortParams &distCoeffs)
{
    // BatchImagePointsとBatchSpacePointsのサイズチェック
    assert(batchSpacePoints.size() == batchImagePoints.size());
    int batchNum = batchSpacePoints.size();
    for (int i = 0; i < batchNum; ++i)
    {
        // 各視点画像内でSpacePointとImagePointは同じ数必要.
        assert(batchSpacePoints[i].size() == batchImagePoints[i].size());
    }

    /*各視点画像ごとにPnP問題を解く*/
    int batchSize = batchSpacePoints.size();
    std::vector<RotVector> rotVecs;
    std::vector<TranVector> tranVecs;
    for (int batchIndex = 0; batchIndex < batchSize; ++batchIndex)
    {
        cv::vec3f rvec, tvec;
        bool bRet = cv::solvePnP(
            batchSpacePoints.at(batchIndex),
            batchImagePoints.at(batchIndex),
            rvec,
            tvec,
            false,
            cv::SOLVEPNP_ITERATIVE);

        if (bRet)
        {
            std::cout << "[" << batchIndex << "th] Success." << std::endl;
            rotVecs.emplace_back(rvec.at<float>[0], rvec.at<float>[1], rvec.at<float>[2]);
            tranVecs.emplace_back(tvec.at<float>[0], tvec.at<float>[1], tvec.at<float>[2]);
        }
        else
        {
            std::cout << "[" << batchIndex << "th] Fail." << stdd:endl;
        }
    }
}
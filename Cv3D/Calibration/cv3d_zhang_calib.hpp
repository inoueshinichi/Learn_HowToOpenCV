/**
 * @file cv3d_zhang_calib.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <opencv4/opencv2/calib3d.hpp> // calibrateCamera
#include <opencv4/opencv2/core/types.hpp> // TermCriteria

#include <cv3d_def.hpp>

#include <iostream>
#include <sstream>


// ZhangCalibの戻り値
struct RetZhangCalib
{
    CamIntrinsicParams mCamInParams;
    LenDistortParams mLenDisParams;
    std::vector<RotVector> mRetRotations;
    std::vector<Translation> mTranslations;

    TransformViewMatrix3x4 GetTransformViewMatrix3x4(int index)
    {
        assert(index >= 0);
        assert(index < mRetRotVectors.size());

        TransformViewMatrix viewMat;

        return viewMat;
    }

    CamExtrinsicMatrix3x4 GetExtrinsicMatrix3x4(int index)
    {
        assert(index >= 0);
        assert(index < mRetRotVectors.size());


    }
};


/*キャリブレーション*/
RetZhangCalib
CalibZhang(
    const BatchSpacePoints &batchSpacePoints,
    const BatchImagePoints &batchImagePoints,
    const ImageShape &imageShape,
    CamIntrinsicParams *initCamInParams)
{
    /*OpenCV API*/
    // https://docs.opencv.org/4.7.0/d9/d0c/group__calib3d.html#ga687a1ab946686f0d85ae0363b5af1d7b
    // TermCliteriaクラス https://docs.opencv.org/4.7.0/d9/d5d/classcv_1_1TermCriteria.html
    // 魚眼はこっち. https://docs.opencv.org/4.7.0/db/d58/group__calib3d__fisheye.html
    // 参考. https://kamino.hatenablog.com/entry/opencv_calibrate_camera
    // double cv::calibrateCamera(InputArrayOfArrays objectPoints,
    //                            InputArrayOfArrays imagePoints,
    //                            Size imageSize,
    //                            InputOutputArray cameraMatrix,
    //                            InputOutputArray distCoeffs,
    //                            OutputArrayOfArrays rvecs,
    //                            OutputArrayOfArrays tvecs,
    //                            OutputArray stdDeviationsIntrinsics,
    //                            OutputArray stdDeviationsExtrinsics,
    //                            OutputArray perViewErrors,
    //                            int flags = 0,
    //                            TermCriteria criteria = TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, DBL_EPSILON))
    // ret : 各視点の再投影誤差の和 (単位:ピクセル)
    // ||Ax-y||^2 : 再投影誤差のL2ノルム -> 最小化の非線形目的関数 -> LM法(勾配法と大体同じ) : バンドル調整
    // 非線形最適化のため, 局所解に陥る事がある. そのため, 最適解を求めるガイドラインとして内部パラメータの初期値を与えると最適化されやすい.
    // cv::calibrateCamera関数でも最初に初期解を推定して, この初期解(推定解)を用いてLM法で最適解を求める2段構えとなっている.
    // 外部パラメータの初期解は, 同一平面上(キャリブレーションパターン面)にあるモデル座標から画像平面へのホモグラフィを考えることで求めている.
    // 非平面のキャリブレーションパターンの場合, 外部パラメータの初期解をホモグラフィで求められないので, 内部パラメータの初期解を与えて, LM法を解いて,
    // 一旦, 外部パラメータの初期解を求めて, 再度, 内部パラメータと外部パラメータを与えて, LM法で最終的な最適化を行っている.

    /**
     * @brief
     * @note objectPoints : std::vector<std::vector<cv::Point3f>>に準ずる型, 各点(x,y,z)はz=0.
     * @note imagePoints : std::vector<std::vector<cv::Point2f>>に準ずる型
     * @note imageSize : カメラの内部パラメータの初期化のみに使用する.
     * @note cameraMatrix : [CALIB_USE_INTRINSIC_GUESS|CALIB_FIX_ASPECT_RATIO|CALIB_FIX_PRINCIPAL_POINT|CALIB_FIX_FOCAL_LENGTH]
     *                      のどれかを指定すると, cameraMatrixの各パラメータ(fx,fy,cx,cy)の一部または全てを`cv::calibrateCamera`を呼ぶ前に
     *                      初期化しなければならない.
     * @note distCoeffs : レンズ歪みパラメータ
     * @note rvecs : 回転ベクトル i-th
     * @note tvecs : 並進ベクトル i-th
     * @note stdDeviationsIntrinsics : 内部パラメータの標準偏差
     * @note stdDeviationsExtrinsics : 外部パラメータの標準偏差
     * @note perViewErrors : 各視点における再投影誤差
     * @note flags : 最適化フラグ
     */

    /**
     * @brief Zhang法による多視点同一オブジェクト画像を用いたカメラキャリブレーション
     * @note OpenCVの座標系は右手系で, 視線方向がZ軸正の向き
     * @カメラ座標系:
     *             Z軸正 : 視線方向
     *             Y軸正 : カメラの下方向(Y軸負がカメラ上方向)
     *             Y軸正 : 右方向
     *
     *
     *
     * [キャリブレーションの成果物]
     * 1. カメラの内部パラメータ (1セットのみ)
     *    ax(fx/dx), ay(fy/dy), cx, cy
     *
     * 2. ローカル座標系(キャリブレーションボード上に設定)とカメラ座標系の間のカメラ外部パラメータ (視点数セット得られる)
     *    取得できる回転成分は, 回転ベクトルで出力される.
     *    rvecs: 回転ベクトルの配列.
     *    tvecs: 並進ベクトルの配列.
     *    cv2.Rodrigues()で方向余弦行列に変換できる.
     *    (rvecs, tvecs)は, カメラ座標系に対するローカル座標系の位置と姿勢 -> ローカル座標系の座標値をカメラ座標系の座標値に変換できる.
     *    X_cam = R @ X_local + T
     *
     * 3. レンズ歪みパラメータ (1セットのみ)
     *    (k1,k2,p1,p2[,k3[,k4,k5,k6[,s1,s2,s3,s4[,tauX,tauY]]]]). 4, 5, 8, 12 or 14要素
     *
     * 4. 推定されたカメラの内部パラメータの各標準偏差. 推定されないパラメータに関してはゼロになる.
     *    (fx, fy, cx, cy, k1, k2, p1, p2, k3, k4, k5, k6, s1, s2, s3, s4, tauX, tauY) fx, fyは実際 fx/delta_x, fy/delta_y
     *
     * 5. 推定されたカメラの外部パラメータの各標準偏差. 推定されないパラメータに関してはゼロになる.
     *    (R_0, t_0, R_1, t_1, ..., R_M-1, t_M-1) Mは使用した画像の枚数.
     *    R_iは(1x3)の回転ベクトル.
     *    t_iは(1x3)の並進ベクトル.
     *
     * 6. 各視点毎の再投影誤差
     *    この数値が小さいほどキャリブレーションが成功している.
     */

    /**
     * @brief LM法(レーベンバック・マッカート)による非線形最適化計算時の設定パラメータ
     * @note flagsの論理和で指定.
     *
     * @brief CALIB_USE_INTRINSIC_GUESS:
     * @note 指定するとカメラの内部パラメータ行列の初期値としてユーザー指定のパラメータ(fx,fy,cx,cy)を使用する.
     * @note 指定しない場合は(cx,cy)を画像中心とし, (fx,fy)は最小二乗法によって求められる.
     * @warning もし完全な内部パラメータが既知ならば, 代わりに`solvePnP`関数を使用すること.
     *
     * @brief CALIB_FIX_INTRINSIC:
     * @note カメラの内部パラメータ行列を固定して最適化計算する.
     *
     * @brief CALIB_SAME_FOCAL_LENGTH:
     * @note 焦点深度(fx,fy)を同じ値で使用する?
     *
     * @brief CALIB_FIX_PRINCIPAL_POINT:
     * @note CALIB_USE_INTRINSIC_GUESSがTrueならば, 最適化中に画像中心(cx,cy)を固定する.
     *
     * @brief CALIB_FIX_ASPECT_RATIO:
     * @note fyのみパラメータとする. fx/fyの比率は初期値と同じ.
     * @warning CALIB_USE_INTRINSIC_GUESSが設定されていない場合, fx,fyの入力値は無視されて比率のみが計算される.
     *
     * @brief CALIB_ZERO_TANGENT_DIST:
     * @note 歪曲収差の接線方向の歪み係数(p1,p2)がゼロに設定される. つまり, 接線方向歪みを考慮しない.
     *
     * @brief CALIB_FIX_TANGENT_DIST:
     * @note 歪曲収差の接線方向の歪み係数(p1,p2)を固定する.
     * @warning CALIB_USE_INTRINSIX_GUESSが指定されている場合, 推定したレンズ歪みの値を使用する.
     *
     * @brief CALIB_FIX_FOCAL_LENGTH:
     * @note CALIB_USE_INTRINSIC_GUESSがTrueならば, 固定光学主点位置(焦点距離＋繰り出し量)を固定する.
     *
     * @brief [CALIB_FIX_K1 | CALIB_FIX_K2 | CALIB_FIX_K3 | CALIB_FIX_K4 | CALIB_FIX_K5 | CALIB_FIX_K6]
     * @note 放射状歪み(半径方向歪み)k[i]を固定する.
     * @warning CALIB_USE_INTRINSIX_GUESSが指定されている場合, 推定したレンズ歪みの値(distCoeffs)を使用する.
     *
     * @brief CALIB_RATIONAL_MODEL:
     * @note 放射状歪みパラメータのk4,k5,k6を計算して返す. 指定しない場合, 放射状歪みに関しては, k1,k2,k3だけが計算される.
     *
     * @brief CALIB_THIN_PRISM_MODEL:
     * @note 薄プリズム歪みパラメータs1,s2,s3,s4を計算して返す. 指定しない場合, k1,k2,k3,p1,p2を計算して返そうとする.
     *
     * @brief CALIB_FIX_S1_S2_S3_S4:
     * @note 薄プリズム歪みパラメータs1,s2,s3,s4を固定する.
     * @warning CALIB_USE_INTRINSIX_GUESSが指定されている場合, 推定したレンズ歪みの値を使用する.
     *
     * @brief CALIB_TILTED_MODEL:
     * @note シャインルーフの原理を利用したレンズと撮像素子の傾斜パラメータtauX,tauYが有効になる. 指定しない場合, k1,k2,k3,p1,p2を計算して返す.
     *
     * @brief CALIB_FIX_TAUX_TAUY:
     * @note シャインルーフ傾斜パラメータtauX,tauYを固定する.
     * @warning CALIB_USE_INTRINSIX_GUESSが指定されている場合, 推定したレンズ歪みの値を使用する.
     *
     * @brief CALIB_ZERO_DISPARITY:
     * @note 未調査
     *
     * @brief CALIB_USE_LU:
     * @note 最適化にLU分解を使用する. Ax=y, min ||Ax-y||^2
     * @warning 最適化対象の連立方程式の行列係数Aが正則なら高速に計算できる.
     *
     * @brief CALIB_USE_QR:
     * @note 最適化にQR分解を使用する. Ax=y, min ||Ax-y||^2
     * @warning 行列係数Aがフルランクなら高速?
     *
     */

    // BatchImagePointsとBatchSpacePointsのサイズチェック
    assert(batchSpacePoints.size() == batchImagePoints.size());
    int batchNum = batchSpacePoints.size();
    for (int i = 0; i < batchNum; ++i)
    {
        // 各視点画像内でSpacePointとImagePointは同じ数必要.
        assert(batchSpacePoints[i].size() == batchImagePoints[i].size());
    }

    // LM法最適化フラグ
    int optimFlags = 0;
    optimFlags |= cv::CALIB_RATIONAL_MODEL;
    optimFlags |= cv::CALIB_THIN_PRISM_MODEL;

    cv::Mat inCamMat = cv::Mat::zeros(3, 3, CV_32FC1);

    if (initCamInParams)
    {
        // 内部パラメータの初期値を使用する
        optimFlags |= cv::CALIB_USE_INTRINSIC_GUESS;

        // 内部パラメータの初期化
        inCamMat.at<double>(0, 0) = initCamInParams->mForcasX;
        inCamMat.at<double>(1, 1) = initCamInParams->mForcasY;
        inCamMat.at<double>(2, 2) = 1.0;
        inCamMat.at<double>(0, 2) = initCamInParams->mCenterX;
        inCamMat.at<double>(1, 2) = initCamInParams->mCenterY;
    }

    /**
     * @brief TermCriteria : 最適化の反復を終了する基準設定
     * @note LM法の反復最適化条件(明らかに反復数が足りていない等のケースを除いてデフォルトのままでOK)
     * @note cv::TermCriteria::Type::COUNT : 指定された回数(count) に到達したら繰り返し計算を終了する.
     * @note cv::TermCriteria::Type::MAX_ITER : cv::TermCirteria::Type::COUNTと同じ.
     * @note cv::TermCriteria::Type::EPS : 指定された制度(epsilon)に到達したら反復計算を終了する.
     */

    // cv::TermCriteria::Type type = cv::TermCriteria::Type::COUNT + cv::TermCriteria::EPS;
    // int maxCount = 30;
    // double epsilon = 0.001;
    // cv::TermCriteria criteriaLM(type, maxCount, epsilon);
    cv::TermCriteria criteriaLM(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, DBL_EPSILON);

    cv::Mat distCoeffs;
    std::vector<cv::Mat> rotVecs; // Rot vectors N*(1,3)
    std::vector<cv::Mat> tranVecs; // Tran vectors N*(1,3)
    cv::Mat stdDeviationsIntrinsics;
    cv::Mat stdDeviationsExtrinsics;
    cv::Mat perViewErrors;

    double retAllRMS = cv::calibrateCamera(
        batchSpacePoints,
        batchImagePoints,
        cv::Size(imageShape.mWidth, imageShape.mHeight),
        inCamMat,
        distCoeffs,
        rotVecs,
        tranVecs,
        stdDeviationsIntrinsics,
        stdDeviationsExtrinsics,
        perViewErrors,
        optimFlags,
        criteriaLM
    );

    std::ostringstream oss;
    oss << "[Input] " << batchNum << "'s images for calibration." << std::endl;

    
    oss << "[Output : RotVec and TranVec]" << std::endl;
    for (int i = 0; i < batchNum; ++i)
    {
        oss << "[" << i << "th]" << std::endl;
        oss << "RotVec: \n" << rotVecs.at(i) << std::endl;
        oss << "TranVec: \n" << tranVecs.at(i) << std::endl;
    }

    oss << "[Output : ]"

    return RetZhangCalib; // 仮
} // CalibZhang
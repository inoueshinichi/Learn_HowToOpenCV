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

#include <cv3d_def_ponts.hpp>

// カメラ内部パラメータ
struct CamIntrinsicParams
{
    double mForcasX;
    double mForcasY;
    double mCenterX;   // 画像中心 X
    double mCenterY;   // 画像中心 Y
    double mTiltPlane; // 理想投影面に対する実際の投影面の傾き
    double mSkew;      // mTilePlane=0のとき, 0
};

// レンズ収差
// (k1,k2,p1,p2[,k3[,k4,k5,k6[,s1,s2,s3,s4[,tauX,tauY]]]]). 4, 5, 8, 12 or 14要素
struct LenDistortParams
{
    double mK1;
    double mK2;
    double mP1;
    double mP2;
    double mK3;
    double mK4;
    double mK5;
    double mK6;
    double mS1;
    double mS2;
    double mS3;
    double mS4;
    double mTauX;
    double mTauY;
};

// カメラ外部パラメータ
struct CamExtrinsicParams
{
    double mR11;
    double mR12;
    double mR13;
    double mR21;
    double mR22;
    double mR23;
    double mR31;
    double mR32;
    double mR33;
    double mTx;
    double mTy;
    double mTz;
};

// カメラ座標への透視投影変換行列
struct TransformViewMatrix
{
    // 行列形状(3, 4). 同次座標系形式. 列優先データ(C/C++). 列優先表現.
    double mP11; // R11
    double mP12; // R12
    double mP13; // R13
    double mP14; // Tx
    double mP21; // R21
    double mP22; // R22
    double mP23; // R23
    double mP24; // Ty
    double mP31; // R31
    double mP32; // R32
    double mP33; // R33
    double mP34; // Tx1
};


// ZhangCalibの戻り値
struct RetZhangCalib
{
    CamIntrinsicParams mRetCamInParams;
    LenDistortParams mRetLenDisParams;
    std::vector<CamExtrinsicParams> mRetCamExParamsVec;

    TransformViewMatrix GetTransformView(int index);
};

#include <opencv4/opencv2/calib3d.hpp>

/*キャリブレーション*/
RetZhangCalib 
CalibZhang(const BatchImagePoints &batchImagePoints, 
           const BatchSpacePoints &batchSpacePoints,
           const CamIntrinsicParams &initCamInParams,
           int height,
           int width)
{
    /*OpenCV API*/
    // https://docs.opencv.org/4.7.0/d9/d0c/group__calib3d.html#ga687a1ab946686f0d85ae0363b5af1d7b
    // TermCliteriaクラス https://docs.opencv.org/4.7.0/d9/d5d/classcv_1_1TermCriteria.html
    // 魚眼はこっち. https://docs.opencv.org/4.7.0/db/d58/group__calib3d__fisheye.html
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
    // ret : 各視点の再投影誤差の和

    /**
     * @brief
     * @note objectPoints : std::vector<std::vector<>>, 各点(x,y,z)はz=0.
     * @note imagePoints : std::vector<std::vector<>>
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
     *
     */
}
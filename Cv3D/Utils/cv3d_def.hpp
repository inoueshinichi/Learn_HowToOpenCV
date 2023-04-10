/**
 * @file cv3d_def_points.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <cmath>
#include <cstring>

// Vector2
struct Vector2
{
    float mX;
    float mY;

    Vector2() : mX(0.f), mY(0.f) {}
    Vector2(float x, float y) : mX(x), mY(y) {}
    virtual ~Vector2() {}

    float NormL2Sq()
    {
        return mX * mX + mY * mY;
    }

    void Normalize()
    {
        float length = std::sqrtf(NormL2Sq());
        if (!length)
            return;
        mX /= length;
        mY /= length;
    }
};

// Vector3
struct Vector3
{
    float mX;
    float mY;
    float mZ;

    Vector3() : mX(0.f), mY(0.f), mZ(0.f) {}
    Vector3(float x, float y, float z) : mX(x), mY(y), mZ(z) {}
    virtual ~Vector3() {}

    float NormL2Sq()
    {
        return mX * mX + mY * mY + mZ * mZ;
    }

    void Normalize()
    {
        float length = std::sqrtf(NormL2Sq());
        if (!length)
            return;
        mX /= length;
        mY /= length;
        mZ /= length;
    }
};

// スケール
struct Scale : public Vector3
{
    Scale() {}
    Scale(float sx, float sy, float sz) : Vector3(sx, sy, sz) {}
    ~Scale() {}
};

// 並進
struct Translation : public Vector3
{
    Translation() {}
    Translation(float tx, float ty, float tz) : Vector3(tx, ty, tz) {}
    ~Translation() {}
};


// Matrix3x3
struct Matrix3x3
{
    float mMat[3][3];

    Matrix3x3() {}
    Matrix3x3(float (&mat)[3][3])
    {
        std::memcpy((void *)mMat, (void *)mat, 9 * sizeof(float));
    }
    ~Matrix3x3() {}
};

// 回転行列
struct Rotation
{
    // 行列形状(3,3). 行優先データ(C/C++). 列優先表現.
    float mR11;
    float mR12;
    float mR13;
    float mR21;
    float mR22;
    float mR23;
    float mR31;
    float mR32;
    float mR33;

    Rotation() {}
    Rotation(float (&mat)[3][3])
    {
        mR11 = mat[0][0];
        mR12 = mat[0][1];
        mR13 = mat[0][2];
        mR21 = mat[1][0];
        mR22 = mat[1][1];
        mR23 = mat[1][2];
        mR31 = mat[2][0];
        mR32 = mat[2][1];
        mR33 = mat[2][2];
    }
    Rotation(const Vector3 &xVec, const Vector3 &yVec, const Vector3 &zVec)
    {
        mR11 = xVec.mX;
        mR21 = xVec.mY;
        mR31 = xVec.mZ;
        mR12 = yVec.mX;
        mR22 = yVec.mY;
        mR32 = yVec.mZ;
        mR13 = zVec.mX;
        mR23 = zVec.mY;
        mR33 = zVec.mZ;
    }
    ~Rotation() {}

    Vector3 GetAxisX() const
    {
        Vector3 axisX(mR11, mR21, mR31);
        axisX.Normalize();
        return axisX;
    }

    Vector3 GetAxisY() const
    {
        Vector3 axisY(mR21, mR22, mR32);
        axisY.Normalize();
        return axisY;
    }

    Vector3 GetAxisZ() const
    {
        Vector3 axisZ(mR31, mR32, mR33);
        axisZ.Normalize();
        return axisZ;
    }
};

// カメラ内部パラメータ
struct CamIntrinsicParams
{
    float mFx;
    float mFy;
    float mCx;   // 画像中心 X
    float mCy;   // 画像中心 Y
    float mPlaneTilt; // 理想投影面に対する実際の投影面の傾き
    float mSkew; // mTilePlane=0のとき, 0

    CamIntrinsicParams() : mFx(0.f), mFy(0.f), mCx(0.f), mCy(0.f), mPlaneTilt(0.f), mSkew(0.f) {}
    CamIntrinsicParams(float fx, float fy, float cx, float cy, float planeTilt=0.f)
        : mFx(fx), mFy(fy), mCx(cx), mCy(cy), mPlaneTilt(planeTilt), mSkew(mFx / std::cos(planeTilt))
    {}
    ~CamIntrinsicParams() {}


    Matrix3x3 GetMatrix3x3()
    {
        float mat[3][3];
        mat[0][0] = mFx;
        mat[0][1] = mSkew;
        mat[0][2] = mCx;
        mat[1][0] = 0.f;
        mat[1][1] = mFy;
        mat[1][2] = mCy;
        mat[2][0] = 0.f;
        mat[2][1] = 0.f;
        mat[2][2] = 1.f;
        return Matrix3x3(mat);
    }
};

#include <unordered_map>
#include <string>

// レンズ収差
// (k1,k2,p1,p2[,k3[,k4,k5,k6[,s1,s2,s3,s4[,tauX,tauY]]]]). 4, 5, 8, 12 or 14要素
struct LenDistortParams
{
    std::unordered_map<std::string, float> distortDict;

    LenDistortParams()
    {
        distortDict.insert( {"K1", 0.0f} );
        distortDict.insert( {"K2", 0.0f} );
        distortDict.insert( {"P1", 0.0f} );
        distortDict.insert( {"P2", 0.0f} );
        distortDict.insert( {"K3", 0.0f} );
        distortDict.insert( {"K4", 0.0f} );
        distortDict.insert( {"K5", 0.0f} );
        distortDict.insert( {"K6", 0.0f} );
        distortDict.insert( {"S1", 0.0f} );
        distortDict.insert( {"S2", 0.0f} );
        distortDict.insert( {"S3", 0.0f} );
        distortDict.insert( {"S4", 0.0f} );
        distortDict.insert( {"TauX", 0.0f} );
        distortDict.insert( {"TauY", 0.0f} );
    }
};

// カメラ外部パラメータ
struct CamExtrinsicMatrix3x4
{
    // 行列形状(3,4). 同次座標系形式. 行優先データ(C/C++). 列優先表現.
    float mR11;
    float mR12;
    float mR13;
    float mTx;
    float mR21;
    float mR22;
    float mR23;
    float mTy;
    float mR31;
    float mR32;
    float mR33;
    float mTz;

    const float* GetFloatPointer()
    {
        return &mR11;
    }

    // https://www.delftstack.com/ja/howto/cpp/how-to-pass-2d-array-to-function-in-cpp/
    void GetMatrix3x4(float (&mat)[3][4])
    {
        mat[0][0] = mR11;
        mat[0][1] = mR12;
        mat[0][2] = mR13;
        mat[0][3] = mTx;
        mat[1][0] = mR21;
        mat[1][1] = mR22;
        mat[1][2] = mR23;
        mat[1][3] = mTy;
        mat[2][0] = mR31;
        mat[2][1] = mR32;
        mat[2][2] = mR33;
        mat[2][3] = mTz;
    }

    Scale GetScale()
    {
        Vector3 xVec(mR11, mR21, mR31);
        float xS = std::sqrtf(xVec.NormL2Sq());
        Vector3 yVec(mR12, mR22, mR32);
        float yS = std::sqrtf(yVec.NormL2Sq());
        Vector3 zVec(mR13, mR23, mR33);
        float zS = std::sqrtf(zVec.NormL2Sq());
        return Scale(xS, yS, zS);
    }

    Rotation GetRotation()
    {
        Vector3 xVec(mR11, mR21, mR31);
        xVec.Normalize();
        Vector3 yVec(mR12, mR22, mR32);
        yVec.Normalize();
        Vector3 zVec(mR13, mR23, mR33);
        zVec.Normalize();
        return Rotation(xVec, yVec, zVec);
    }

    Translation GetTranslation()
    {
        return Translation(mTx, mTy, mTz);
    }
};

// 画像形状
struct ImageShape
{
    int mChannels;
    int mHeight;
    int mWidth;
};

// Image Point
struct ImagePoint : public Vector2
{
    ImagePoint(float x, float y) : Vector2(x, y) {}
    ~ImagePoint() {}
};

// Space Point
struct SpacePoint : public Vector3
{
    SpacePoint(float x, float y, float z) : Vector3(x, y, z) {}
    ~SpacePoint() {}
};

#include <vector>

// Per one image
using ImagePoints = std::vector<ImagePoint>;
using SpacePoints = std::vector<SpacePoint>;

// Batch of ImagePoints and SpacePoints
using BatchImagePoints = std::vector<ImagePoints>;
using BatchSpacePoints = std::vector<SpacePoints>;

// 透視投影変換行列
struct ProjectionMatrix3x4
{
    // 行列形状(3,4). 同次座標系形式. 行優先データ(C/C++). 列優先表現.
    float mP11; // R11
    float mP12; // R12
    float mP13; // R13
    float mP14; // Tx
    float mP21; // R21
    float mP22; // R22
    float mP23; // R23
    float mP24; // Ty
    float mP31; // R31
    float mP32; // R32
    float mP33; // R33
    float mP34; // Tx1

    ProjectionMatrix3x4()
        : mP11(0.f), mP12(0.f), mP13(0.f), mP14(0.f)
        , mP21(0.f), mP22(0.f), mP23(0.f), mP24(0.f)
        , mP31(0.f), mP32(0.f), mP33(0.f), mP34(0.f) 
    {}

    ProjectionMatrix3x4(const float (&mat)[3][4])
    {
        SetMatrix3x4(mat);
    }

    const float *GetFloatPointer()
    {
        return &mP11;
    }

    void SetMatrix3x4(const float (&mat)[3][4])
    {
        mP11 = mat[0][0];
        mP12 = mat[0][1];
        mP13 = mat[0][2];
        mP14 = mat[0][3];
        mP21 = mat[1][0];
        mP22 = mat[1][1];
        mP23 = mat[1][2];
        mP24 = mat[1][3];
        mP31 = mat[2][0];
        mP32 = mat[2][1];
        mP33 = mat[2][2];
        mP34 = mat[2][3];
    }


    void GetMatrix3x4(float (&mat)[3][4])
    {
        mat[0][0] = mP11;
        mat[0][1] = mP12;
        mat[0][2] = mP13;
        mat[0][3] = mP14;
        mat[1][0] = mP21;
        mat[1][1] = mP22;
        mat[1][2] = mP23;
        mat[1][3] = mP24;
        mat[2][0] = mP31;
        mat[2][1] = mP32;
        mat[2][2] = mP33;
        mat[2][3] = mP34;
    }

    ImagePoints Projection(const SpacePoints& spacePoints)
    {
        ImagePoints imgPts;
        int numPts = spacePoints.size();
        imgPts.reserve(numPts);
        imgPts.resize(numPts);
        float S, u, v;
        for (int i = 0; i < numPts; ++i)
        {
            S = u = v = 0.f;
            float x, y, z;
            x = spacePoints[i].mX;
            y = spacePoints[i].mY;
            z = spacePoints[i].mZ;
            S = mP31 * x + mP32 * y + mP33 * z + mP34;
            u = mP11 * x + mP12 * y + mP13 * z + mP14;
            v = mP21 * x + mP22 * y + mP23 * z + mP24;
            u /= S;
            v /= S;
            imgPts.push_back(ImagePoint(u, v));
        }
        return imgPts;
    }
};
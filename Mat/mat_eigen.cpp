/**
 * @file mat_eigen.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief Eigenライブラリとの互換性確認
 * @version 0.1
 * @date 2023-08-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

/**
 * @brief Eigenのベクトル・行列へのマッピング用クラス
 *
 */
#include <Eigen/Dense>
template <typename T>
using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
template <typename T>
using RowVector = Eigen::Matrix<T, 1, Eigen::Dynamic>;
template <typename T>
using ColVector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
template <typename T>
using MatrixMap = Eigen::Map<Matrix<T>>;
template <typename T>
using RowVectorMap = Eigen::Map<RowVector<T>>;
template <typename T>
using ColVectorMap = Eigen::Map<ColVector<T>>;
template <typename T>
using ConstMatrixMap = Eigen::Map<const Matrix<T>>;
template <typename T>
using ConstRowVectorMap = Eigen::Map<const RowVector<T>>;
template <typename T>
using ConstColVectorMap = Eigen::Map<const ColVector<T>>;
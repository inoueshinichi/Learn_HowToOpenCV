/**
 * @file mat_gen.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief cv::Matの初期化, 生成方法
 * @version 0.1
 * @date 2023-08-13
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

auto main(int, char **) -> int
{
    try
    {
        // 行ベクトル
        {
            // from array
            {
                unsigned char v[] = { 1,2,3,4,5,6,7,8,9 };
                cv::Mat vec01 = cv::Mat(1, sizeof(v)/sizeof(unsigned char), CV_8U, v);
                std::cout << cv::format(vec01, cv::Formatter::FMT_NUMPY) << ";" << std::endl;
            }

            // zeros
            {
                cv::Mat vec02 = cv::Mat::zeros(1, 5, CV_8U);
                std::cout << cv::format(vec02, cv::Formatter::FMT_NUMPY) << ";" << std::endl;
            }

            // ones
            {
                cv::Mat vec03 = cv::Mat::ones(1, 5, CV_8U);
                std::cout << cv::format(vec03, cv::Formatter::FMT_NUMPY) << ";" << std::endl;
            }

            // scalar
            {
                // cv::Mat vec04 = cv::Mat()
            }
        }
        // cv::Mat dispMat = cv::Mat::zeros(3, 3, CV_32FC1);
        // std::cout << "dispMat :\n"
        //           << dispMat << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
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


auto main(int, char **) -> int
{
    try
    {
        /* code */
        cv::Mat dispMat = cv::Mat::zeros(3, 3, CV_32FC1);
        std::cout << "dispMat :\n"
                  << dispMat << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
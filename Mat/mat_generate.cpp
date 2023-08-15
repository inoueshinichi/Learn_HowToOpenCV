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
/**
 * @file ostu_binarize.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv)
{
    try
    {
        // テスト画像
        std::string test_file = GetTestData("nekosan.jpg");
        std::cout << "Test file path: " << test_file << std::endl;

        // 画像読み込み
        cv::Mat img_src;
        img_src = cv::imread(test_file, cv::IMREAD_GRAYSCALE);
        if (img_src.empty())
            throw("failed open file.");
        CV_IMSHOW(img_src)

        // 大津の2値化
        cv::Mat img_bin;
        double thresh = cv::threshold(img_src, img_bin, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        CV_IMSHOW(img_bin)
        std::printf("Ostu threshold : %f\n", thresh);

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
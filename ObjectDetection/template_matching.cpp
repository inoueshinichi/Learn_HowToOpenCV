/**
 * @file template_matching.cpp
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
        // 画像読み込み
        std::string test_file1 = GetTestData("catmod.jpg");
        std::string test_file2 = GetTestData("cat.jpg");
        std::cout << "Test file 1 path: " << test_file1 << std::endl;
        std::cout << "Test file 2 path: " << test_file2 << std::endl;

        std::vector<cv::Mat> img_src_vec;
        img_src_vec.reserve(2);
        cv::Mat img_in;
        img_in = cv::imread(test_file1, cv::IMREAD_COLOR);
        img_src_vec.emplace_back(img_in);
        img_in = cv::imread(test_file2, cv::IMREAD_COLOR);
        img_src_vec.emplace_back(img_in);
        if (img_src_vec.empty())
            throw("failed open file.");
        std::printf("Got %ld images\n", img_src_vec.size());

        int rows1 = img_src_vec[0].rows;
        int cols1 = img_src_vec[0].cols;
        int rows2 = img_src_vec[1].rows;
        int cols2 = img_src_vec[1].cols;
        int ch1 = img_src_vec[0].channels();
        int ch2 = img_src_vec[1].channels();

        if (rows1 > rows2)
        {
            std::printf("template-rows1: %d, search-rows2: %d\n", rows1, rows2);
            throw("template-rows1 > search-rows2");
        }
        if (cols1 > cols2)
        {
            std::printf("template-cols1: %d, search-cols2: %d\n", cols1, cols2);
            throw("template-cols1 > search-cols2");
        }

        if (ch1 != ch2)
        {
            std::printf("ch1: %d, ch2: %d\n", ch1, ch2);
            throw("ch1 != ch2");
        }

        // shallow copy
        cv::Mat img_template = img_src_vec[0];
        cv::Mat img_search = img_src_vec[1];

        CV_TYPE_LOG(img_template)
        CV_TYPE_LOG(img_search)

        

        CV_IMSHOW(img_template)
        CV_IMSHOW(img_search)
        // CV_IMSHOW()

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
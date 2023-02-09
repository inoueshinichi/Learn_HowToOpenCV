/**
 * @file mask_add.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-09
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
        if (rows1 != rows2)
        {
            std::printf("rows1: %d, rows2: %d\n", rows1, rows2);
            throw("rows1 != rows2");
        }
        if (cols1 != cols2)
        {
            std::printf("cols1: %d, cols2: %d\n", cols1, cols2);
            throw("cols1 != cols2");
        }
        if (ch1 != ch2)
        {
            std::printf("ch1: %d, ch2: %d\n", ch1, ch2);
            throw("ch1 != ch2");
        }

        CV_TYPE_LOG(img_src_vec[0])
        CV_TYPE_LOG(img_src_vec[1])

        int width = cols1;
        int height = rows1;
        int channels = ch1;

        // 指定領域
        cv::Mat img_mask = cv::Mat::zeros(cv::Size(width, height), CV_8UC1);
        cv::Point p1(width/4, height/4);
        cv::Point p2(width*3/4, height*3/4);
        cv::rectangle(img_mask, p1, p2, cv::Scalar(255), -1);
        CV_IMSHOW(img_mask)

        // 指定領域の抽出
        cv::Mat img_masked;
        img_src_vec[0].copyTo(img_masked, img_mask);
        CV_IMSHOW(img_masked)

        // 指定領域外の抽出
        cv::Mat img_ng_mask;
        cv::bitwise_not(img_mask, img_ng_mask); // 論理否定
        cv::Mat img_ng_masked;
        img_src_vec[0].copyTo(img_ng_masked, img_ng_mask);
        CV_IMSHOW(img_ng_masked)

        // 指定領域の足し算
        cv::Mat img_mask_add = cv::Mat::zeros(cv::Size(width, height), img_src_vec[0].type());
        cv::add(img_src_vec[0], img_src_vec[1], img_mask_add, img_mask);
        CV_IMSHOW(img_mask_add)

        // 指定領域で論理積
        cv::Mat img_masked_and;
        cv::bitwise_and(img_src_vec[0], img_src_vec[1], img_masked_and, img_mask);
        CV_IMSHOW(img_masked_and)

        // 指定領域での論理和
        cv::Mat img_masked_or;
        cv::bitwise_or(img_src_vec[0], img_src_vec[1], img_masked_or, img_mask);
        CV_IMSHOW(img_masked_or)

        // 指定領域での排他的論理和
        cv::Mat img_masked_xor;
        cv::bitwise_xor(img_src_vec[0], img_src_vec[1], img_masked_xor, img_mask);
        CV_IMSHOW(img_masked_xor)

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
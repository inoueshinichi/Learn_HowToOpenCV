/**
 * @file alpha_brending.cpp
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

        // アルファブレンド
        cv::Mat img_alpha_brend = cv::Mat::zeros(cv::Size(width, height), img_src_vec[0].type());
        const double alpha = 0.5;

// #define MY_ALPHA_BREND
#if defined(MY_ALPHA_BREND)

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                for (int c = 0; c < channels; ++c)
                {
                    uchar v1 = img_src_vec[0].data[y * img_src_vec[0].step + x * channels + c];
                    uchar v2 = img_src_vec[1].data[y * img_src_vec[1].step + x * channels + c];
                    img_alpha_brend.data[y * img_alpha_brend.step + x * channels + c] =
                        cv::saturate_cast<uchar>(alpha * v1 + (1.0 - alpha) * v2);
                }
            }
        }
#else

        // OpenCVによるアルファブレンディング
        cv::addWeighted(img_src_vec[0], alpha, img_src_vec[1], (1 - alpha), 0, img_alpha_brend);

#endif
        CV_IMSHOW(img_src_vec[0])
        CV_IMSHOW(img_src_vec[1])
        CV_IMSHOW(img_alpha_brend)

        cv::waitKey(0);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}
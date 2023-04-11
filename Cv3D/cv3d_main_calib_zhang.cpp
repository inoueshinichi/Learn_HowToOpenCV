/**
 * @file cv3d_main_calib_zhang.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief キャリブレーション
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>


#include <cv3d_def.hpp>
#include <cv3d_zhang_calib.hpp>

auto main(int, char**) -> int
{
    try
    {
        /* code */
        std::cout << "Hello World" << std::endl;

        // キャリブレーションパターンのグリッド数
        std::tuple<int, int> numGridTuple;
        std::get<0>(numGridTuple) = 0;
        std::get<1>(numGridTuple) = 0;
        std::string calibPattern;
        float gridSpan = 0.0; // [mm]
#if (0)
        calibPattern = "SquareGridPattern";
        numGridTuple = std::make_pair(5, 7);
#else
        calibPattern = "CircleGridPattern";
        numGridTuple = std::make_pair(3, 5);
#endif

        /*キャリブレーションパターン画像の読み込み*/
        std::string calibPatternDir = GetTestData("Calib3DPatterns");     // Src
        std::string resultCalibPatternDir = calibPatternDir + "/Result";  // Dst
        

        if (calibPattern == "CircleGridPattern")
        {

        }
        else
        {

        }

        /*キャリブレーション準備*/


        /*キャリブレーション実行*/

        /*キャリブレーション結果の確認*/
        

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;   
}
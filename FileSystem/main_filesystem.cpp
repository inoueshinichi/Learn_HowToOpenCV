/**
 * @file main_filesystem.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>

#include <filesystem.hpp>
#include <glob.hpp>
#include <open.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <vector>


auto main(int, char**) -> int
{
    try
    {
        // glob
        {
            std::string pattern = GetTestData("*.jpg");
            auto pathList = is::common::glob_paths(pattern);
            std::ostringstream oss;
            oss << "[PathList]\n";
            for (const auto &path : pathList)
            {
                oss << path << std::endl;
            }

            pattern = GetTestData("*.png");
            pathList = is::common::glob_paths(pattern);
            oss << "[PathList]\n";
            for (const auto &path : pathList)
            {
                oss << path << std::endl;
            }
            std::cout << oss.str() << std::endl;

            auto sfp = is::common::fopen(GetTestData("HelloWorld.txt"), "r");
            if (sfp)
            {
                std::cout << "Open " << GetTestData("HelloWorld.txt") << std::endl;
            }
        }

        // mkdir
        {
            std::string 
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}
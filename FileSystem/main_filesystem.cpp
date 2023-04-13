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
#include <mkdir.hpp>
#include <rmdir.hpp>
#include <open.hpp>
#include <rename.hpp>

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

        // rmdir
        {
            std::string dirname = GetTestData("Calib3DPatterns");
            if (is::common::rmdir(dirname))
            {
                std::cout << "Remove directry: " << dirname << std::endl;
            }
            else
            {
                std::cout << "Failed to remove directory: " << dirname << std::endl;
            }
        }

        // mkdir
        {
            std::string dirname = GetTestData("Calib3DPatterns");
            if (is::common::mkdir(dirname))
            {
                std::cout << "Create directry: " << dirname << std::endl;
            }
            else
            {
                std::cout << "Failed to create directory: " << dirname << std::endl;
            }
        }

        // open
        {
            std::string filename = GetTestData("HelloWorld_utf8.txt");
            auto sfp = is::common::fopen(filename, "r");
            char str[256];

            if (sfp)
            {
                fgets(str, sizeof(str) / sizeof(str[0]), sfp.get());
                std::cout << "Load string: " << str << std::endl;
            }
        }

        // rename
        {
            std::string oldname = GetTestData("HelloWorld_utf8.txt");
            std::string newname = GetTestData("HelloWorld.txt");
            if (is::common::rename(oldname, newname))
            {
                std::cout << "Success to rename " << oldname << " to " << newname << std::endl;
            }
            else
            {
                std::cout << "Failed to rename" << std::endl;
            }
        }

        // is_exist
        {
            std::string name = GetTestData("HelloWorld.txt");
            if (is::common::is_exist(name))
            {
                std::cout << "Exist " << name << std::endl;
            }
            else
            {
                std::cout << "No exist " << name << std::endl;
            }
        }

        // is_dir
        {
            std::string dirname = GetTestData("Calib3DPatterns");
            if (is::common::is_dir(dirname))
            {
                std::cout << "Directory " << dirname << std::endl;
            }
            else
            {
                std::cout << "No directory " << dirname << std::endl;
            }

            std::string name = GetTestData("HelloWorld.txt");
            if (is::common::is_dir(name))
            {
                std::cout << "Directory " << name << std::endl;
            }
            else
            {
                std::cout << "No directory " << name << std::endl;
            }
        }

        // is_file
        {
            std::string dirname = GetTestData("Calib3DPatterns");
            if (is::common::is_file(dirname))
            {
                std::cout << "File " << dirname << std::endl;
            }
            else
            {
                std::cout << "No file " << dirname << std::endl;
            }

            std::string name = GetTestData("HelloWorld.txt");
            if (is::common::is_file(name))
            {
                std::cout << "File " << name << std::endl;
            }
            else
            {
                std::cout << "No file " << name << std::endl;
            }
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}
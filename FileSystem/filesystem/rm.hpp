/**
 * @file rm.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <filesystem.hpp>

#include <string>

namespace is
{
    namespace common
    {
        inline bool 
        rm(const std::string& filename)
        {
            bool ret = false;
            int stat = remove(filename.c_str());
            if (stat == 0)
            {
                ret = true;
            }
            return ret;
        }
    }
}
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
        namespace detail
        {
#if defined(_WIN32) || defined(_WIN64)
    #if defined(_UNICODE) || defined(UNICODE)
            inline bool
            __rm(const std::string &filename)
            {
                bool ret = false;
                std::wstring filenameL = is::common::cvt_shiftjis_to_utf16(filename);
                int stat = _wremove(filenameL.c_str());
                if (stat == 0)
                {
                    ret = true;
                }
                return ret;
            }
    #else
            inline bool
            __rm(const std::string &filename)
            {
                bool ret = false;
                int stat = ::remove(filename.c_str());
                if (stat == 0)
                {
                    ret = true;
                }
                return ret;
            }
    #endif
#else
            inline bool
            __rm(const std::string &filename)
            {
                bool ret = false;
                int stat = ::remove(filename.c_str());
                if (stat == 0)
                {
                    ret = true;
                }
                return ret;
            }
#endif
        }

        inline bool
        rm(const std::string &filename)
        {
            return detail::__rm(filename);
        }
        
    }
}
/**
 * @file rmdir.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <common.hpp>

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
            __rmdir(const std::string &dirname)
            {
                bool ret = false;
                std::wstring dirnameL = is::common::cvt_shiftjis_to_utf16(dirname);
                int stat = _wrmdir(dirnameL.c_str());
                if (stat == 0)
                {
                    ret = true;
                }
                return ret;
            }
#else
            inline bool 
            __rmdir(const std::string &dirname)
            {
                bool ret = false;
                int stat = _rmdir(dirname.c_str());
                if (stat == 0)
                {
                    ret = true;
                }
                return ret;
            }
#endif

#elif defined(__linux__) || defined(__MACH__)
            inline bool 
            __rmdir(const std::string &dirname)
            {
                bool ret = false;
                int stat = ::rmdir(dirname.c_str());
                if (stat == 0)
                {
                    ret = true;
                }
                return ret;
            }

#else
#error "Not match platform"
#endif
        }

        inline bool 
        rmdir(const std::string& dirname)
        {
            return detail::__rmdir(dirname);
        }
    }
}
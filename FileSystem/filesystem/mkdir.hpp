/**
 * @file mkdir.hpp
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
            __mkdir(const std::string& dirname)
            {
                bool ret = false;
                std::wstring dirnameL = is::common::cvt_shiftjis_to_utf16(dirname);
                // MSVCはアクセス権の設定がない.
                if (_wmkdir(dirnameL.c_str()) == 0)
                {
                    ret = true;
                }
                return ret;
            }
#else
            inline bool
            __mkdir(const std::string& dirname)
            {
                bool ret = false;
                // MSVCはアクセス権の設定がない.
                if (_mkdir(dirname.c_str()) == 0)
                {
                    ret = true;
                }
                return ret;
            }
#endif

#elif defined(__linux__) || defined(__MACH__)

            inline bool
            __mkdir(const std::string &dirname)
            {
                bool ret = false;
                mode_t mode;
                /*アクセス権*/
                // 所有者は読み書き実行可能, 所有者以外は, アクセス不可とする.
                mode = S_IRUSR | S_IWUSR | S_IXUSR;
                if (::mkdir(dirname.c_str(), mode) == 0)
                {
                    ret = true;
                }
                return ret;
            }

#endif
        }

        inline bool 
        mkdir(const std::string& dirname)
        {
            return detail::__mkdir(dirname);
        }
    }
}
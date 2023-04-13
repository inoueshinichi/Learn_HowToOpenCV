/**
 * @file rename.hpp
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
#include <status.hpp>

#include <string>

namespace is
{
    namespace common
    {
        namespace detail
        {
#if defined(_WIN32) || defined(_WIN64)
            inline bool
            __rename(const std::string& oldname, const std::string& newname)
            {
                bool ret = false;
                int stat = ::rename(oldname.c_str(), newname.c_str());
                if (stat == 0) // newnameと重複するファイル名, ディレクトリ名が存在する場合失敗する.
                {
                    ret = true;
                }
                return ret;
            }

#elif defined(__linux__) || defined(__MACH__)
            inline bool
            __rename(const std::string &oldname, const std::string &newname)
            {
                // macOS, Linux(Ubuntu)ではデフォルトで上書き設定なので, 
                // newnameが既に存在する場合, 失敗とする.
                bool stat_ret = is_exist(newname);
                if (stat_ret)
                {
                    return false;
                }
                bool ret = false;
                int stat = ::rename(oldname.c_str(), newname.c_str());
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
        rename(const std::string& oldname, const std::string& newname)
        {
            return detail::__rename(oldname, newname);
        }
    }
}
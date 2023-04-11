/**
 * @file glob.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <filesystem.hpp>

#include <vector>
#include <string>

namespace is
{
    namespace common
    {
        namespace detail
        {
#if defined(_WIN32) || defined(_WIN64)

#elif defined(__linux__) || defined(__MACH__)
            /**
             * @brief glob
             * @note https://linuxjm.osdn.jp/html/LDP_man-pages/man3/glob.3.html
             * @note #include <glob.h>
             */
            // typedef struct
            // {
            //     size_t gl_pathc; /* 今までにマッチしたパスの数 */
            //     char **gl_pathv; /* マッチしたパス名のリスト */
            //     size_t gl_offs;  /* gl_pathv 内に確保するスロット数 */
            // } glob_t;
            //
            // int glob(const char *pattern, int flags,
            //          int (*errfunc)(const char *epath, int eerrno),
            //          glob_t *pglob);
            //
            // void globfree(glob_t *pglob);
            std::vector<std::string>
            GetGlobPaths(const std::string &filePathPattern)
            {
                std::vector<std::string> pathList;
                glob_t globbuf;

                /**
                 * @brief ret 戻り値
                 * 成功 : 0
                 * メモリを使い果たした : GLOB_NOSPACE
                 * 読み取りエラー : GLOB_ABORTED
                 * 一つもマッチしなかった : GLOB_NOMATCH
                 */
                int ret = glob(filePathPattern.c_str(), 0, nullptr, &globbuf);
                size_t numPaths = globbuf.gl_pathc;
                for (size_t i = 0; i < numPaths; ++i)
                {
                    pathList.push_back(globbuf.gl_pathv[i]);
                }
                globfree(&globbuf);

                return pathList;
            }
#else
#error "Not match platform."
#endif
        } // detail

        std::vector<std::string> 
        GetGlobPaths(const std::string &filePathPattern)
        {
            return detail::GetGlobPaths(filePathPattern);
        }
    }
}
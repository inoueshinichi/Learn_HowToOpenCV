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
#include <transform_char.hpp>

#include <vector>
#include <string>

namespace is
{
    namespace common
    {
        namespace detail
        {
#if defined(_WIN32) || defined(_WIN64)
            /**
             * @brief WIN32APIを使用する
             * @note https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-findfirstfilea
             * @note https://learn.microsoft.com/ja-jp/windows/win32/api/minwinbase/ns-minwinbase-win32_find_dataa
             * @warning ANSIバージョンでは, 名前は`MAX_PATH`文字に制限されている.
             */
            inline std::vector<std::string>
            __glob_paths(const std::string &pattern)
            {
                /*UTF-16バージョンを使用する*/
                HANDLE hFindFile;
                WIN32_FIND_DATAW stFindData;
                std::vector<std::string> pathlist;

                std::wstring patternL = is::common::cvt_shiftjis_to_utf16(pattern);

                hFindFile = ::FindFirstFileW(patternL.c_str(), &stFindData);
                if (hFindFile == INVALID_HANDLE_VALUE)
                {
                    FindClose(hFindFile);
                    return pathlist;
                }

                while (::FindNextFileW(hFindFile, &stFindData))
                {
                    std::string path = is::common::cvt_utf16_to_shiftjis(stFindData.cFileName);
                    pathlist.push_back(path);
                }

                FindClose(hFindFile);
                return pathlist;
            }


#elif defined(__linux__) || defined(__MACH__)
            /**
             * @brief glob
             * @note https://linuxjm.osdn.jp/html/LDP_man-pages/man3/glob.3.html
             * @note #include <glob.h>
             */
            inline std::vector<std::string>
            __glob_paths(const std::string &pattern)
            {
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

                std::vector<std::string> pathlist;
                glob_t globbuf;

                /**
                 * @brief ret 戻り値
                 * 成功 : 0
                 * メモリを使い果たした : GLOB_NOSPACE
                 * 読み取りエラー : GLOB_ABORTED
                 * 一つもマッチしなかった : GLOB_NOMATCH
                 */
                int ret = glob(pattern.c_str(), 0, nullptr, &globbuf);
                if (ret != 0)
                {
                    // メモリ解放
                    globfree(&globbuf);
                    return pathlist;
                }

                size_t num_paths = globbuf.gl_pathc;
                for (size_t i = 0; i < num_paths; ++i)
                {
                    pathlist.push_back(globbuf.gl_pathv[i]);
                }

                // メモリ解放
                globfree(&globbuf);

                return pathlist;
            }
#else
#error "Not match platform."
#endif
        } // detail

        inline std::vector<std::string>
        glob_paths(const std::string &pattern)
        {
            return detail::__glob_paths(pattern);
        }
    }
}
/**
 * @file status.hpp
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
#include <transform_char.hpp>

#include <string>

namespace is
{
    namespace common
    {
        /**
         * @brief EntryStatus
         * @note https://learn.microsoft.com/ja-jp/cpp/c-runtime-library/reference/stat-functions?view=msvc-170
         * @note https://linuxjm.osdn.jp/html/LDP_man-pages/man2/stat.2.html
         * @note https://www.c-lang.net/stat/index.html
         */
        struct EntryStatus
        {
            dev_t st_dev; /* ファイルがあるデバイスの ID */       // Windowsでは, ドライブ番号.
            ino_t st_ino;                                         /* inode 番号 */
            mode_t st_mode;                                       /* アクセス保護 */
            nlink_t st_nlink; /* ハードリンクの数 */              // Windowsでは, 非NTFSファイルシステムでは常に`1`.
            uid_t st_uid; /* 所有者のユーザー ID */               // Windowsでは, 常に`0`.
            gid_t st_gid; /* 所有者のグループ ID */               // Windowsでは, 常に`0`.
            dev_t st_rdev; /* デバイス ID (特殊ファイルの場合) */ // Windowsでは, `st_dev`と同じ.
            off_t st_size;                                        /* 全体のサイズ (バイト単位) */
            blksize_t st_blksize;                                 /* ファイルシステム I/O での ブロックサイズ */
            blkcnt_t st_blocks;                                   /* 割り当てられた 512B のブロック数 */
            std::string st_brth_dt; /* 作成時刻 */                // WindowsとmacOSのみ有効.
            std::string st_acc_dt; /* 最終アクセス時刻 */         // Windowsでは, NTFSの場合有効. FAT形式の場合無効.
            std::string st_mod_dt; /* 最終修正時刻 */             // Windowsでは, NTFSの場合有効. FAT形式の場合無効.
            std::string st_ch_dt; /* 最終状態変更時刻 */          // Windowsでは, NTFSの場合有効. FAT形式の場合無効.
        };

        namespace detail
        {
#if defined(_WIN32) || defined(_WIN64)
            inline bool
            __status(const std::string &entryname, EntryStatus &entrystatus, bool slink)
            {
                std::memset(&entrystatus, 0, sizeof(EntryStatus));
                bool ret = false;
                struct __stat64 stat;

                if (slink)
                {
                    if (_stat64(entryname.c_str(), &stat) == 0)
                    {
                        /*ここでエントリーの情報を取得*/
                        ret = true;
                    }
                }
                else
                {
                    /**
                     * @brief WIN32APIを使用する
                     * @note https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-getfileattributesa
                     * @note https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-getfileattributesw
                     * @note https://learn.microsoft.com/ja-jp/windows/win32/fileio/file-attribute-constants
                     * @warning ANSIバージョンでは、名前は`MAX_PATH`文字に制限されている.
                     */
                    std::wstring entrynameL = is::common::cvt_shiftjis_to_utf16(entryname);
                    DWORD file_attributes = GetFileAttributesW(entrynameL.c_str());

                    /*ここでエントリーの情報を取得*/
                    ret = true;
                }
                
                return ret;
            }

            inline bool
            __is_dir(const std::string &entryname)
            {
                EntryStatus status;
                bool ret = detail::__status(entryname, status, true);
                if (!ret)
                    return false;
                if ((status.st_mode & _S_IFMT) == _S_IFDIR)
                    return true;
                else
                    return false;
            }

            inline bool
            __is_file(const std::string &entryname)
            {
                EntryStatus status;
                bool ret = detail::__status(entryname, status, true);
                if (!ret)
                    return false;
                if ((status.st_mode & _S_IFMT) == _S_IFREG)
                    return true;
                else
                    return false;
            }

            inline bool
            __is_slink(const std::string &entryname)
            {
                EntryStatus status;
                bool ret = detail::__status(entryname, status, true);
                if (!ret)
                    return false;
                if ((status.st_mode & _S_IFMT) == _S_IFLNK)
                    return true;
                else
                    return false;
            }

#elif defined(__MACH__)
            inline bool
            __status(const std::string &entryname, EntryStatus &entrystatus, bool slink)
            {
                std::memset(&entrystatus, 0, sizeof(EntryStatus));
                bool ret = false;
                struct stat64 stat;
                int stat_ret = 0;
                if (slink)
                {
                    // シンボリックリンクを辿る
                    stat_ret = stat64(entryname.c_str(), &stat);
                }
                else
                {
                    // シンボリックリンクを辿らない. シンボリックリンク自体のファイル情報を取得する.
                    stat_ret = lstat64(entryname.c_str(), &stat);
                }

                if (stat_ret == 0)
                {
                    /*ここでエントリーの情報を取得*/
                    entrystatus.st_dev = stat.st_dev;
                    entrystatus.st_ino = stat.st_ino;

                    ret = true;
                }

                return ret;
            }

            inline bool
            __is_dir(const std::string &entryname)
            {
                EntryStatus status;
                bool ret = is::common::detail::__status(entryname, status, true);
                if (!ret)
                    return false;
                if ((status.st_mode & S_IFMT) == S_IFDIR)
                    return true;
                else
                    return false;
            }

            inline bool
            __is_file(const std::string &entryname)
            {
                EntryStatus status;
                bool ret = is::common::detail::__status(entryname, status, true);
                if (!ret)
                    return false;
                if ((status.st_mode & S_IFMT) == S_IFREG)
                    return true;
                else
                    return false;
            }

            inline bool
            __is_slink(const std::string &entryname)
            {
                EntryStatus status;
                bool ret = is::common::detail::__status(entryname, status, true);
                if (!ret)
                    return false;
                if ((status.st_mode & S_IFMT) == S_IFLNK)
                    return true;
                else
                    return false;
            }

#elif defined(__linux__)
            inline bool
            __status(const std::string &entryname, EntryStatus &entrystatus, bool slink)
            {
                bool ret = false;
                struct stat64 stat;
                if (slink)
                {
                    // シンボリックリンクを辿る
                    stat_ret = stat64(entryname.c_str(), &stat);
                }
                else
                {
                    // シンボリックリンクを辿らない. シンボリックリンク自体のファイル情報を取得する.
                    stat_ret = lstat64(entryname.c_str(), &stat);
                }

                if (stat_ret == 0)
                {
                    /*ここでエントリーの情報を取得*/
                    ret = true;
                }

                return ret;
            }

#else
#error "Not match platform"
#endif
        }

        inline bool
        status(const std::string &entryname, EntryStatus &entrystatus, bool slink = true)
        {
            return detail::__status(entryname, entrystatus, slink);
        }

        inline bool
        is_exist(const std::string &entryname)
        {
            EntryStatus status;
            return detail::__status(entryname.c_str(), status, true);
        }

        inline bool
        is_dir(const std::string &entryname)
        {
            return detail::__is_dir(entryname);
        }

        inline bool
        is_file(const std::string &entryname)
        {
            return detail::__is_file(entryname);
        }

        inline bool
        is_slink(const std::string &entryname)
        {
            return detail::__is_slink(entryname);
        }
    }
}
/**
 * @file filesystem.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

// Distinguish OS Platform
#if defined(_WIN32) || defined(_WIN64)
#include <fcntl.h> // open
#include <io.h> // open
#include <sys/stat.h> // open, __stat64, _stat64
#include <direct.h> // _mkdir, _rmdir, _getcwd, _getdrive, _chdir, _chdrive
#include <wchar.h> // _tマクロ

#elif defined(__linux__) || defined(__MACH__)
#include <fcntl.h> // open
#include <sys/stat.h> // mkdir, stat, stat64
#include <sys/types.h>
#include <unistd.h> // rmdir, getcwd, chdir, open

#include <dirent.h>
#include <glob.h>

#else
#error "Not match platform"
#endif

// Prohibit 32bit env
#if defined(_WIN32) && !defined(_WIN64)
#error "Please select 64bit environment. Prohibit 32bit environment."
#elif defined(__linux__)
    #if defined(ILP32) || defined(LP32)
        #error "Please select 64bit environment. Prohibit 32bit environment."
    #endif
#endif

// Check Data type model
// https://www.wdic.org/w/TECH/データ型モデル
#if defined(_WIN32) || defined(_WIN64)
    #if defined(LLP64)

    #endif
#elif defined(__linux__)
    #if defined(LP64) || defined(ILP64)

    #endif
#endif

#include <stdio.h> // FILE, fopen, fclose, remove
#include <errno.h> // errno_t, clearerr

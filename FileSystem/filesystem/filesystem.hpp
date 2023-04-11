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

#if defined(_WIN32) || defined(_WIN64)
#include <fcntl.h> // open
#include <io.h> // open
#include <sys/stat.h> // open
#include <direct.h> // _mkdir, _rmdir, _getcwd, _getdrive, _chdir, _chdrive

#elif defined(__linux__) || defined(__MACH__)
#include <fcntl.h> // open
#include <sys/stat.h> // mkdir
#include <sys/types.h>
#include <unistd.h>   // rmdir, getcwd, chdir, open
#include <dirent.h>
#include <glob.h>

#else
#error "Not match platform"
#endif

#include <stdio.h> // FILE, fopen, fclose

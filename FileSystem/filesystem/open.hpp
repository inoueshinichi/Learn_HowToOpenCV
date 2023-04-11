/**
 * @file open.hpp
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

#include <memory>
#include <string>

namespace is
{
    namespace common
    {
        namespace detail
        {
#if defined(_WIN32) || defined(_WIN64)
            std::shared_ptr<FILE> fopen(const std::string& filename, const std::string& mode, bool utf8)
            {
                auto deleter = [](FILE* fp) { if (fp) fclose(fp); };
                FILE* fp;
                std::shared_ptr<FILE> sfp(nullptr, deleter);
                int fd, option, pmode;

                // modeの解析
                if (mode.find("r") != std::string::npos)
                {
                    option |= _O_RDONLY;
                }
                if (mode.find("w") != std::string::npos)
                {
                    option |= _O_CREAT | _O_TRUNC | _O_WRONLY;
                }
                if (mode.find("a") != std::string::npos)
                {
                    option |= _O_APPEND;
                }

                if (mode.find("b" != std::string::npos))
                {
                    option |= _O_BINARY;
                }
                else
                {
                    option |= _O_TEXT;
                }

                if (utf8)
                {
                    option |= _U_U8TEXT;
                }
                
                // 新規作成ファイルに対するアクセス権限
                pmode = _S_IREAD | _S_IWRITE;

                fd = _open(filename.c_str(), option, pmode);
                if (fd != -1)
                {
                    fp = _fdopen(fd, mode.c_str());
                    if (!fp)
                    {
                        sfp.reset(fp);
                    }
                    else
                    {
                        _close(fd);
                    }
                }
                return sfp;
            }


#elif defined(__linux__) || defined(__MACH__)
            std::shared_ptr<FILE> fopen(const std::string& filename, const std::string& mode, bool utf8)
            {
                auto deleter = [](FILE* fp) { if (fp) fclose(fp); };
                FILE* fp;
                std::shared_ptr<FILE> sfp(nullptr, deleter);
                int fd, option, pmode;

                // modeの解析
                if (mode.find("r") != std::string::npos)
                {
                    option |= O_RDONLY;
                }
                if (mode.find("w") != std::string::npos)
                {
                    option |= O_CREAT | O_TRUNC | O_WRONLY;
                }
                if (mode.find("a") != std::string::npos)
                {
                    option |= O_APPEND;
                }
                
                // 新規作成ファイルに対するアクセス権限
                pmode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // chomodと同じ

                fd = open(filename.c_str(), option, pmode);

                if (fd != -1)
                {
                    fp = fdopen(fd, mode.c_str());
                    if (!fp)
                    {
                        sfp.reset(fp);
                    }
                    else
                    {
                        close(fd);
                    }
                }
                return sfp;
            }
#else
#error "Not match platform"
#endif
        }

        std::shared_ptr<FILE> fopen(const std::string& filename, const std::string& mode, bool utf8 = true)
        {
            return detail::fopen(filename, mode, utf8);
        }
    }
}
/**
 * @file transform_char.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#if defined(_WIN32) || defined(_WIN64)

#include <string>
#include <iostream>
#include <iomanip> // std::setfill, std::setw

namespace is
{
    namespace common
    {
        //////////////////////////////////
        /* Convert from UTF-8 to UTF-16 */
        //////////////////////////////////
        inline std::wstring cvt_utf8_to_utf16(char const *utf8_char);
        inline std::wstring cvt_utf8_to_utf16(const std::string &utf8_string);

        //////////////////////////////////
        /* Convert from UTF-16 to UTF-8 */
        //////////////////////////////////
        inline std::string cvt_utf16_to_utf8(const wchar_t *utf16_wchar);
        inline std::string cvt_utf16_to_utf8(const std::wstring &utf16_wstring);
        inline std::string cvt_utf16_to_utf8(const char16_t *utf16_char16);
        inline std::string cvt_utf16_to_utf8(const std::u16string &utf16_string);

        //////////////////////////////////////
        /* Convert from Shift-JIS to UTF-16 */
        //////////////////////////////////////
        inline std::wstring cvt_shiftjis_to_utf16(const char *shiftjis_char);
        inline std::wstring cvt_shiftjis_to_utf16(const std::string &shiftjis_string);

        //////////////////////////////////////
        /* Convert from UTF-16 to Shift-JIS */
        //////////////////////////////////////
        inline std::string cvt_utf16_to_shiftjis(const wchar_t *utf16_wchar);
        inline std::string cvt_utf16_to_shiftjis(const std::wstring &utf16_wstring);
        inline std::string cvt_utf16_to_shiftjis(const char16_t *utf16_char16);
        inline std::string cvt_utf16_to_shiftjis(const std::u16string &utf16_string);

        //////////////////////////////////////
        /* Convert from UTT-8 to Shift-JIS  */
        //////////////////////////////////////
        inline std::string cvt_utf8_to_shiftjis(const char *utf8_char);
        inline std::string cvt_utf8_to_shiftjis(const std::string &utf8_string);

        //////////////////////////////////////
        /* Convert from Shift-JIS to UTF-8  */
        //////////////////////////////////////
        inline std::string cvt_shiftjis_to_utf8(const char *shiftjis_char);
        inline std::string cvt_shiftjis_to_utf8(const std::string &shiftjis_string);

        //////////////////
        /* 16進数でダンプ */
        //////////////////
        template <typename OSTREAM, typename T>
        void dump_by_hex(OSTREAM &os, T const *t)
        {
            bool is_first = true;
            const auto *byte = reinterpret_cast<unsigned char const *>(t);
            os << std::hex << std::uppercase << std::setfill('0') << std::setw(2);
            while (*byte)
            {
                if (!std::exchange(is_first, false))
                {
                    os << " ";
                }
                os << static_cast<unsigned int>(*byte);
                ++byte;
            }
            os << std::endl;
        }
    }
}

#endif
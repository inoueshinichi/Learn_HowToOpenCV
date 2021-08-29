//
//  mat_traits.h
//  matProject
//
//  Created by Inoue Shinichi on 2019/01/02.
//  Copyright © 2019 Inoue Shinichi. All rights reserved.
//

#ifndef mat_traits_h
#define mat_traits_h
#include "mat_def.h"

namespace my
{
    template<typename T>
    class CDataType
    {
    public:
        typedef T value_type;
        typedef value_type work_type;
        typedef value_type channel_type;
        typedef value_type vec_type;
        enum
        {
            generic_type = 1,
            depth = -1,
            channels = 1,
            fmt = 0,
            type = MY_MAKETYPE(depth, channels)
        };
    };
    
    
    template<>
    class CDataType<bool>
    {
    public:
        typedef bool value_type;
        typedef int work_type;
        typedef value_type channel_type;
        typedef value_type vec_type;
        enum
        {
            generic_type = 0,
            depth = MY_8U,
            channels = 1,
            fmt = (int)'u',
            type = MY_MAKETYPE(depth, channels)
        };
    };
    
    
    template<>
    class CDataType<uchar>
    {
    public:
        typedef uchar value_type;
        typedef int work_type;
        typedef value_type channel_type;
        typedef value_type vec_type;
        enum
        {
            generic_type = 0,
            depth = MY_8U,
            channels = 1,
            fmt = (int)'u',
            type = MY_MAKETYPE(depth, channels)
        };
    };
    
    
    template<>
    class CDataType<char>
    {
    public:
        typedef char value_type;
        typedef int work_type;
        typedef value_type channel_type;
        typedef value_type vec_type;
        enum
        {
            generic_type = 0,
            depth = MY_8S,
            channels = 1,
            fmt = (int)'c',
            type = MY_MAKETYPE(depth, channels)
        };
    };
    
    
    template<>
    class CDataType<ushort>
    {
    public:
        typedef ushort value_type;
        typedef int work_type;
        typedef value_type channel_type;
        typedef value_type vec_type;
        enum
        {
            generic_type = 0,
            depth = MY_16U,
            channels = 1,
            fmt = (int)'w',
            type = MY_MAKETYPE(depth, channels)
        };
    };
    
    
    template<>
    class CDataType<short>
    {
    public:
        typedef short value_type;
        typedef int work_type;
        typedef value_type channel_type;
        typedef value_type vec_type;
        enum
        {
            generic_type = 0,
            depth = MY_16S,
            channels = 1,
            fmt = (int)'s',
            type = MY_MAKETYPE(depth, channels)
        };
    };
    
    
    template<>
    class CDataType<int>
    {
    public:
        typedef int value_type;
        typedef value_type work_type;
        typedef value_type channel_type;
        typedef value_type vec_type;
        enum
        {
            generic_type = 0,
            depth = MY_32S,
            channels = 1,
            fmt = (int)'i',
            type = MY_MAKETYPE(depth, channels)
        };
    };
    
    
    template<>
    class CDataType<float>
    {
    public:
        typedef float value_type;
        typedef value_type work_type;
        typedef value_type channel_type;
        typedef value_type vec_type;
        enum
        {
            generic_type = 0,
            depth = MY_32F,
            channels = 1,
            fmt = (int)'f',
            type = MY_MAKETYPE(depth, channels)
        };
    };
    
    
    template<>
    class CDataType<double>
    {
    public:
        typedef double value_type;
        typedef value_type work_type;
        typedef value_type channel_type;
        typedef value_type vec_type;
        enum
        {
            generic_type = 0,
            depth = MY_64F,
            channels = 1,
            fmt = (int)'d',
            type = MY_MAKETYPE(depth, channels)
        };
    };
    
    
    template<typename T>
    class CDataDepth
    {
    public:
        enum
        {
            value = CDataType<T>::depth,
            fmt = CDataType<T>::fmt
        };
    };
    
    
    
}



#endif /* mat_traits_h */

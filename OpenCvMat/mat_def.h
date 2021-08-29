//
//  mat_def.h
//  matProject
//
//  Created by Inoue Shinichi on 2018/12/28.
//  Copyright © 2018 Inoue Shinichi. All rights reserved.
//

#ifndef mat_def_h
#define mat_def_h

// 型の下限・上限
#define MY_MIN_UCHAR 0
#define MY_MAX_UCHAR 255
#define MY_MIN_CHAR -128
#define MY_MAX_CHAR 127
#define MY_MIN_USHORT 0
#define MY_MAX_USHORT 65535
#define MY_MIN_SHORT -32768 // 2byte
#define MY_MAX_SHORT 32767
#define MY_MIN_ULONG 0
#define MY_MAX_ULONG 4294967295
#define MY_MIN_LONG -2147483648 // 4byte
#define MY_MAX_LONG 2147583647
#define MY_MIN_FLOAT 1.175494351E-38
#define MY_MAX_FLOAT 3.402823466E+38
#define MY_MIN_DOUBLE 2.2250738585072014E-308
#define MY_MAX_DOUBLE 1.7976931348623158E+308

// 数値定数
#define MY_PI   3.1415926535897932384626433832795
#define MY_2PI  6.283185307179586476925286766559
#define MY_LOG2 0.69314718055994530941723212145818

// Mat情報
#define MY_CN_MAX 512 // 0x00000200
#define MY_CN_SHIFT 3 // 0x00000003
#define MY_DEPTH_MAX (1 << MY_CN_SHIFT) // 0x00000008
#define MY_MAT_DEPTH_MASK (MY_DEPTH_MAX - 1) // 0x00000007
#define MY_MAT_DEPTH(flags) ((flags) & MY_MAT_DEPTH_MASK) // depth情報を抽出
#define MY_MAKETYPE(depth,cn) (MY_MAT_DEPTH(depth) + (((cn) - 1) << MY_CN_SHIFT))
#define MY_MAT_CONT_FLAG_SHIFT 14
#define MY_MAT_CONT_FLAG (1 << MY_MAT_CONT_FLAG_SHIFT)
#define MY_SUBMAT_FLAG_SHIFT 15
#define MY_SUBMAT_FLAG (1 << MY_SUBMAT_FLAG_SHIFT)
#define MY_MAT_TYPE_MASK (MY_DEPTH_MAX * MY_CN_MAX - 1) // bit field 12以下のすべてのマスク
#define MY_MAT_TYPE(flags) ((flags) & MY_MAT_TYPE_MASK)
#define MY_MAT_CN_MASK ((MY_CN_MAX - 1) << MY_CN_SHIFT)
#define MY_MAT_CN(flags) ((((flags) & MY_MAT_CN_MASK) >> MY_CN_SHIFT) + 1)


namespace my
{
    typedef unsigned char byte;
    typedef unsigned char uchar;
    typedef unsigned short ushort;
    typedef float float_32;
    typedef double float_64;
    
    enum Depth
    {
        MY_8U = 0,  // 1byte
        MY_8S = 1,  // 1byte
        MY_16U = 2, // 2byte
        MY_16S = 3, // 2byte
        MY_32S = 4, // 4byte
        MY_32F = 5, // 4byte
        MY_64F = 6, // 8byte
    };
    
    enum DataType
    {
        // MyMatの型
        MY_8UC1 = MY_MAKETYPE(MY_8U, 1), // 0
        MY_8UC2 = MY_MAKETYPE(MY_8U, 2), // 8
        MY_8UC3 = MY_MAKETYPE(MY_8U, 3), // 16
        MY_8UC4 = MY_MAKETYPE(MY_8U, 4), // 24
        
        MY_8SC1 = MY_MAKETYPE(MY_8S, 1), // 1
        MY_8SC2 = MY_MAKETYPE(MY_8S, 2), // 9
        MY_8SC3 = MY_MAKETYPE(MY_8S, 3), // 17
        MY_8SC4 = MY_MAKETYPE(MY_8S, 4), // 25
        
        MY_16UC1 = MY_MAKETYPE(MY_16U, 1), // 2
        MY_16UC2 = MY_MAKETYPE(MY_16U, 2), // 10
        MY_16UC3 = MY_MAKETYPE(MY_16U, 3), // 18
        MY_16UC4 = MY_MAKETYPE(MY_16U, 4), // 26
        
        MY_16SC1 = MY_MAKETYPE(MY_16S, 1), // 3
        MY_16SC2 = MY_MAKETYPE(MY_16S, 2), // 11
        MY_16SC3 = MY_MAKETYPE(MY_16S, 3), // 19
        MY_16SC4 = MY_MAKETYPE(MY_16S, 4), // 27
        
        
        MY_32SC1 = MY_MAKETYPE(MY_32S, 1), // 4
        MY_32SC2 = MY_MAKETYPE(MY_32S, 2), // 12
        MY_32SC3 = MY_MAKETYPE(MY_32S, 3), // 20
        MY_32SC4 = MY_MAKETYPE(MY_32S, 4), // 28
        
        MY_32FC1 = MY_MAKETYPE(MY_32F, 1), // 5
        MY_32FC2 = MY_MAKETYPE(MY_32F, 2), // 13
        MY_32FC3 = MY_MAKETYPE(MY_32F, 3), // 21
        MY_32FC4 = MY_MAKETYPE(MY_32F, 4), // 29
        
        MY_64FC1 = MY_MAKETYPE(MY_64F, 1), // 6
        MY_64FC2 = MY_MAKETYPE(MY_64F, 2), // 14
        MY_64FC3 = MY_MAKETYPE(MY_64F, 3), // 22
        MY_64FC4 = MY_MAKETYPE(MY_64F, 4), // 30
    };
}

#endif /* mat_def_h */

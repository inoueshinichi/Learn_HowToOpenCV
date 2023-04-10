/**
 * @file cv3d_def_points.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

// Image Point
struct ImagePoint
{
    double mU;
    double mV;
};

// Space Point
struct SpacePoint
{
    double mX;
    double mY;
    double mZ;
};

#include <vector>

// Per one image
using ImagePoints = std::vector<ImagePoint>;
using SpacePoints = std::vector<SpacePoint>;

// Batch of ImagePoints and SpacePoints
using BatchImagePoints = std::vector<ImagePoints>;
using BatchSpacePoints = std::vector<SpacePoints>;




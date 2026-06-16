#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <cstdint>

struct CameraFrame
{
    uint64_t timestamp_ns;

    std::string camera_name;

    cv::Mat image;
};

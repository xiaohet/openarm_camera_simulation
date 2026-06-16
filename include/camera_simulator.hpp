#pragma once

#include "camera_frame.hpp"
#include "timestamp.hpp"

class CameraSimulator
{
public:

    CameraSimulator(const std::string& name, int fps);
    CameraFrame capture();
    int getFPS() const;


private:

    std::string name_;

    int fps_;

    int frame_count_;
};

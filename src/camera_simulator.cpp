#include "camera_simulator.hpp"

CameraSimulator::CameraSimulator(const std::string& name, int fps):
name_(name),
fps_(fps),
frame_count_(0)
{
}

CameraFrame CameraSimulator::capture()
{
    cv::Mat image(480, 640, CV_8UC3);

    image.setTo(
        cv::Scalar(rand()%255, rand()%255, rand()%255));

    cv::putText(image, name_, {50,50}, cv::FONT_HERSHEY_SIMPLEX,
        1.0, {255,255,255}, 2);

    CameraFrame frame;

    frame.timestamp_ns = now_ns();
    frame.camera_name = name_;
    frame.image = image;

    return frame;
}

int CameraSimulator::getFPS() const{
    return fps_;
}
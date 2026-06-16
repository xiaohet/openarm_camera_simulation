#pragma once

#include "camera_frame.hpp"
#include "joint_state.hpp"
#include <unordered_map>

class EpisodeRecorder
{
public:
    void addJointState(const JointState& state);
    void addCameraFrame(const CameraFrame& frame);

    void saveJointStates(const std::string& episode_name);
    void saveCameraFrames(const std::string& episode_name);
    void saveMetadata(const std::string& episode_name);
    void save(const std::string& episode_name);
    void saveH5(const std::string& episode_name);

private:
    std::vector<JointState> joint_states_;

    std::unordered_map<std::string, std::vector<CameraFrame>> camera_streams_;
};

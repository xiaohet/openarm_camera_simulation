#include "episode_recorder.hpp"
#include <fstream>
#include <string>
#include <filesystem>
#include <iomanip>
#include <H5Cpp.h>

namespace fs = std::filesystem;
using namespace H5;

void EpisodeRecorder::addJointState(
    const JointState& state)
{
    joint_states_.push_back(state);
}

void EpisodeRecorder::addCameraFrame(
    const CameraFrame& frame)
{
    camera_streams_[frame.camera_name].push_back(frame);
}

void EpisodeRecorder::saveJointStates(const std::string& episode_name){

    std::ofstream joint_file(episode_name + "/joint_states.csv");

    for(const auto& state : joint_states_)
    {
        joint_file << state.timestamp_ns;

        for(float p : state.position)
            joint_file << "," << p;
        for(float v : state.velocity)
            joint_file << "," << v;
        for(float t : state.torque)
            joint_file << "," << t;

        joint_file << "\n";
    }
}

void EpisodeRecorder::saveCameraFrames(const std::string& episode_name){
    
    std::ostringstream filename;

    for(const auto& [camera_name, frames] : camera_streams_)
    {
        fs::create_directories(episode_name + "/" + camera_name);
    }

    // save images
    for(const auto& [camera_name, frames] : camera_streams_)
    {
        for(size_t i=0; i<frames.size(); i++)
        {
            // name images by timestamp values
            filename << episode_name << "/" << camera_name << "/"
                << frames[i].timestamp_ns << ".png";

            cv::imwrite(filename.str(), frames[i].image);
            
            // save frame metadata
            std::ofstream frame_file(episode_name + "/" + camera_name + "/frames.csv");

            frame_file << "frame_id,timestamp_ns\n";
            for(size_t i=0; i<frames.size(); i++)
            {
                frame_file << i << "," << frames[i].timestamp_ns << "\n";
            }
        }   
    }
}

void EpisodeRecorder::saveMetadata(const std::string& episode_name){
    // first calculate number of camera frames
    size_t total_frames = 0;

    for(const auto& [camera_name, frames] : camera_streams_)
    {
        total_frames += frames.size();
    }

    std::ofstream meta_file(episode_name + ".json");

    meta_file << "{\n";
    meta_file << "  \"episode_name\": \"" << episode_name << "\",\n";
    meta_file << "  \"num_joint_states\": " << joint_states_.size() << ",\n";
    meta_file << "  \"num_camera_frames\": " << total_frames << ",\n";
    meta_file << "  \"cameras\":[\n";

    bool first = true;
    for(const auto& [camera_name, frames] : camera_streams_)
    {
        if(!first)
            meta_file << ",\n";
        meta_file << "    \"" << camera_name << "\"";
        first = false;
    }
    meta_file << "\n";
    meta_file << "  ]\n";
    meta_file << "}\n";
}

// save simple structured data
void EpisodeRecorder::save(const std::string& episode_name){
    fs::create_directories(episode_name);

    saveJointStates(episode_name);
    saveCameraFrames(episode_name);
    saveMetadata(episode_name);

    std::cout << "Saving episode to " << episode_name << std::endl;
    std::cout << "Joint states: " << joint_states_.size() << std::endl;
    std::cout  << "Camera streams: " << camera_streams_.size() << std::endl;
}

void EpisodeRecorder::saveH5(const std::string& episode_name){
    H5File file(episode_name + ".h5", H5F_ACC_TRUNC);

    std::string image_root = episode_name + "_images";
    fs::create_directories(image_root);

    Group metadata_group = file.createGroup("/metadata");
    Group joints_group = file.createGroup("/joints");
    Group cameras_group = file.createGroup("/cameras");

    // store joint timestamps
    std::vector<uint64_t> timestamps;
    std::vector<float> positions;
    std::vector<float> velocities;
    std::vector<float> torques;
    for(const auto& state : joint_states_)
    {
        timestamps.push_back(state.timestamp_ns);
        positions.insert(positions.end(), state.position.begin(), state.position.end());
        velocities.insert(velocities.end(), state.velocity.begin(), state.velocity.end());
        torques.insert(torques.end(), state.torque.begin(), state.torque.end());
    }

    hsize_t ts_dims[1] = {timestamps.size()};
    DataSpace ts_space(1, ts_dims);
    DataSet ts_dataset = joints_group.createDataSet(
        "timestamps", PredType::NATIVE_UINT64, ts_space);
    ts_dataset.write(timestamps.data(), PredType::NATIVE_UINT64);

    // store positions, velocities, and torques
    hsize_t pos_dims[2] = {joint_states_.size(), 6};
    DataSpace pos_space(2, pos_dims);
    DataSet pos_dataset = joints_group.createDataSet(
        "positions", PredType::NATIVE_FLOAT, pos_space);
    pos_dataset.write(positions.data(), PredType::NATIVE_FLOAT);

    hsize_t vel_dims[2] = {joint_states_.size(), 6};
    DataSpace vel_space(2, vel_dims);
    DataSet vel_dataset = joints_group.createDataSet(
        "velocities", PredType::NATIVE_FLOAT, vel_space);
    vel_dataset.write(velocities.data(), PredType::NATIVE_FLOAT);

    hsize_t tor_dims[2] = {joint_states_.size(), 6};
    DataSpace tor_space(2, tor_dims);
    DataSet tor_dataset = joints_group.createDataSet(
        "torques", PredType::NATIVE_FLOAT, tor_space);
    tor_dataset.write(torques.data(), PredType::NATIVE_FLOAT);

    // store metadata
    StrType strdatatype(PredType::C_S1, episode_name.size());
    Attribute attr = file.createAttribute(
        "episode_name", strdatatype, DataSpace());
    attr.write(strdatatype, episode_name);

    // 
    for(const auto& [camera_name, frames] : camera_streams_)
    {
        Group cam_group = file.createGroup("/cameras/" + camera_name);
        
        // store camera timestamps 
        std::vector<uint64_t> cam_timestamps;
        for(const auto& frame : frames)
        {
            cam_timestamps.push_back(frame.timestamp_ns);
        }
        
        hsize_t cam_ts_dims[1] = { cam_timestamps.size() };
        DataSpace cam_ts_space(1, cam_ts_dims);
        DataSet cam_ts_dataset = cam_group.createDataSet(
            "timestamps", PredType::NATIVE_UINT64, cam_ts_space);
        cam_ts_dataset.write(cam_timestamps.data(), PredType::NATIVE_UINT64);

        // store images
        std::string camera_dir = image_root + "/" + camera_name;
        fs::create_directories(camera_dir);

        std::vector<std::string> image_paths;
        for(const auto& frame : frames)
        {
            std::string filename = camera_dir + "/"
                + std::to_string(frame.timestamp_ns) + ".png";
            cv::imwrite(filename, frame.image);
            image_paths.push_back(filename);
        }

        // store image paths
        hsize_t path_dims[1] = { image_paths.size() };
        DataSpace path_space(1, path_dims);
        StrType path_type(PredType::C_S1, H5T_VARIABLE);
        DataSet path_dataset = cam_group.createDataSet("image_paths", path_type, path_space);

        std::vector<const char*> c_paths;
        for(const auto& path : image_paths)
        {
            c_paths.push_back(path.c_str());
        }
        path_dataset.write(c_paths.data(), path_type);

    }

    std::cout << "Saving episode to " << episode_name << ".h5" << std::endl;
    std::cout << "Joint states: " << joint_states_.size() << std::endl;
    std::cout  << "Camera streams: " << camera_streams_.size() << std::endl;

}
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <deque>
#include <queue>
#include <mutex>
#include "camera_simulator.hpp"
#include "timestamp.hpp"
#include "joint_simulator.hpp"
#include "synchronizer.hpp"
#include "episode_recorder.hpp"

std::queue<CameraFrame> frame_queue;
std::deque<JointState> joint_buffer;
std::mutex joint_mutex;

EpisodeRecorder recorder;

void camera_loop(CameraSimulator& cam)
{
    // for saving episode data in simulation; in real use, change to infinite loop
    for(int i = 0; i < 300; i++) 
    {
        auto frame = cam.capture();
        recorder.addCameraFrame(frame);

        std::lock_guard<std::mutex> lock(joint_mutex);

        if(joint_buffer.empty())
        {
            std::cout << "No joint samples yet" << std::endl;
            continue;
        }

        JointState nearest = Synchronizer::findNearest(frame.timestamp_ns, joint_buffer);
        std::cout << frame.camera_name << " aligned with " << nearest.timestamp_ns << std::endl;

        frame_queue.push(frame);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / cam.getFPS()));

        std::cout << frame.camera_name << " " << frame.timestamp_ns << std::endl;
    }
}

void joint_loop()
{
    JointSimulator sim;

    // for saving episode data in simulation; in real use, change to infinite loop
    for(int i = 0; i < 1000; i++)
    {
        JointState state = sim.generate();
        recorder.addJointState(state);

        std::lock_guard<std::mutex> lock(joint_mutex);

        joint_buffer.push_back(state);

        if(joint_buffer.size() > 1000)
        {
            joint_buffer.pop_front();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main()
{
    episode_count = 1;
    CameraSimulator left("left_wrist", 30);
    CameraSimulator right("right_wrist", 30);
    CameraSimulator ceiling("ceiling", 15);
    CameraSimulator zed("zed", 60);

    std::thread joint_thread(joint_loop);
    std::thread left_thread(camera_loop, std::ref(left));
    std::thread right_thread(camera_loop, std::ref(right));
    std::thread ceiling_thread(camera_loop, std::ref(ceiling));
    std::thread zed_thread(camera_loop, std::ref(zed));

    joint_thread.join();
    left_thread.join();
    right_thread.join();
    ceiling_thread.join();
    zed_thread.join();

    // for testing saving episode data
    std::this_thread::sleep_for(std::chrono::seconds(5));
    recorder.saveMetadata("episode_0001");
    recorder.saveH5("episode_0001");

    return 0;
}

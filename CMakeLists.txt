cmake_minimum_required(VERSION 3.10)

project(OpenArmCameraSimulation)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

find_package(HDF5 REQUIRED COMPONENTS C CXX)

message(STATUS "HDF5_LIBRARIES=${HDF5_LIBRARIES}")
message(STATUS "HDF5_INCLUDE_DIRS=${HDF5_INCLUDE_DIRS}")

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(
    openarm_sim

    src/main.cpp
    src/camera_simulator.cpp
    src/joint_simulator.cpp
    src/synchronizer.cpp
    src/timestamp.cpp
    src/episode_recorder.cpp
)

target_link_libraries(
    openarm_sim
    ${OpenCV_LIBS}
    ${HDF5_LIBRARIES}
)

target_include_directories(
    openarm_sim PRIVATE
    ${HDF5_INCLUDE_DIRS}
)


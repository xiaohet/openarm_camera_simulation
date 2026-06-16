# openarm_camera_simulation

## Introduction 
This repository contains the camera simulation project for OpenArm 2.0 robotic arm, which is implemented with OpenCV library in C++ for camera simulation, and Flask library in Python for REST API control and dashboard display. This project assumes different frame rates for different cameras (left wrist, right wrist, ceiling, head) between 15~60 fps, so that the timestamp method for synchronizing different camera frames along with joint states is performed. 

The data of each recording episode are saved as an HDF5 file, a metadata file, and frame images. REST API commands are available for listing all episodes, getting the metadata of one episode, and downloading the data of one episode. A dashboard is created to control the recording of cameras and display camera image, joint state and episode information. 

## Camera and Joint Synchronizer 
For synchronizing the data of different cameras, a timestamp--a big value denoting the actual physical time--is given to each camera frame as well as the joint state. The synchronizer is centered on joint states, which are taken at a fixed 10 ms interval. Then, the synchronizer searches for the camera frame with the nearest timestamp for each camera. This method is not only helpful in handling cameras of different frame rates; it is also helpful in case a camera has unstable frame rate. 

## Recording Episode Storage 
One episode of recorded data will be stored in "build" folder. HDF5 is selected as the main structure format given its extensive use in engineering and open-source nature. The HDF5 data is stored in the "<episode_name>.h5" file. Other data stored include a metadata file "<episode_name>.json" and PNG images of frames from each of the four cameras, which are stored in the four subfolders of the "<episode_name>_images" folder, respectively. 

This is a simulation project built completely on software, with generated rather than captured images and joint states. To simulate the episode recording process, camera capture and joint simulation loops are made finite, so that an episode can be recorded at the end of the simulation process. To use this project for real OpenArm robots, change the loops to infinite. 

Note that during running, this project may generate a large number of messages due to continued camera loops after the joint simulation loop is over. It will run approximately 1~2 minutes before terminating and generating episode records. 

## REST API commands 
The REST API can be created through running "api_server.py". 

While running "api_server.py": 

To get list of episodes, use "curl localhost:5000/episodes". 

To retrieve episode metadata, use "curl localhost:5000/episodes/<episode_name>/metadata". 

To download an episode, use "wget localhost:5000/episodes/<episode_name>/download". 

## Dashboard 
To run the dashboard, run "api_server.py" and open "localhost:5000" in a browser. The dashboard should have the following contents: 

Camera Preview: shows the images from different cameras 

Joint States: shows the updated states (position, velocity, torque) of the arm joints. 

Episode Count: shows the index of the episode of the current recording. 

Recording Control: start or stop camera recording. 

Note that, because this is a simulation project, the dashboard has no actual use of controlling the recording of cameras so far. All camera images and joint states currently on display in the dashboard are simulated results. 

## Next Steps 
Since we do not have hardware access at this point, many things are subject to improvements once we have hardware access. We need to set up the CAN interface and read data over CAN FD. Then, we can better test the functionality of the camera and joint loops, the utility of the dashboard, and the structure of more episodes of recorded data. 

In camera synchronization, one thing subject to improvements is the "nearest timestamp" method, whose accuracy could be improved with a linear interpolation method, by taking in the data of two nearest images and weight average them in some way. 

Another issue due to time limits is the episode name, now with fixed name "episode_0001" while there should actually be a counter of episodes so that the name can be updated accordingly.


# Objective:
Detection of stairs using a Microsoft Kinect camera and Opencv libraries.

Project is focussed towards incorporating this technology into prosthetic legs.
Prosthetic legs which are available in the market are usually devoid of any battery or motors or other types of active acutaors.
So, when a patient or a user of these prosthetic legs is walking towards a set of stairs, he or she has to manually swich over 
from 'walking mode' to 'stairs climbing' mode. This could be very inconvenient and also risky at times.
Hence, this project focusses on the development of a **simple algorithm** that can switch this mode autonomously when the user 
is about to climb a stair.
Of course there are very advanced prosthetic legs available as well, but they are usually super expensive.

This project is only to prove that the algorithm works. There is no size limitation on the hardware components to be used.
So use of a big depth camera like the Kinect and a Laptop is permitted. 
Future work will focus on the use of more compact hardware which can be potentially integrated with prosthetic legs.

# Requirements:
* Algorithm should be able to run in real time on a laptop (without any dependence on GPUs). 
* A small model stair is provided (image below). The algorithm should be able to detect that as well as real stairs.
* Algorithm should be able differetiate between stairs going up or down and also measure the distance to the stairs 
when the user is close enough.
* All softwares should be open source.
* Overall setup should be battery operated and should be portable enough for testing on actual stars.

# Current Framework:
**[ Note: Please also check Future Imrovements section. ]**
* Opencv libraies, Ubuntu 16.04.
* Microsoft Kinect XBOX 360, Laptop computer. 
* Model stairs provided:
<img src="https://github.com/abhanjac/stairs_detection_kinect_opencv/blob/master/images/model_stairs.jpg" width="250" height="300">

# Algorithm Description:

### Algorithm for detecting stairs moving down (Down-stairs):
This algorithm only takes into account the first two steps of the stairs. This is done for two reasons. 
1. So that it is able to detect the stairs that actually have only two steps. 
2. As a person climbs down, the number of steps visible to him becomes fewer. And just before reaching the ground, only two 
or three steps might be visible. 

**So, why not make the algorithm try to detect only one step then?**
Detection of a single step is also possible, but there would be a lot of false detections. There are a number of objects
that resembles a stair with a single step, like a concrete beam lying on the ground or the edges of the sidewalk beside the streets, etc. Hence, considering only the first two steps of the stairs, seems to be the optimal choice.

### Preprocessing of the images for down-stairs:
To filter out the unwanted objects like walls or handrails, we consider only the lower central part of the image for our 
analysis (the interest region). This interest region is shown below (right image is **BGR image**, left is **Depth image**).

<img src="https://github.com/abhanjac/stairs_detection_kinect_opencv/blob/master/images/down_stairs_interest_region.png">

### Feature Extraction for “down-stairs”:
Multiple parallel scans of the points in the interest region are taken (the dots in the images).
The depths of the scanned points are plotted against the y-coordinates of their corresponding pixels, which gives a plot like 
the following:

<img src="https://github.com/abhanjac/stairs_detection_kinect_opencv/blob/master/images/down_stairs_depth_vs_y_coordinate.png">

The plot shows a sudden change in depth of the scanned points at the locations corresponding to the edges of the stairs. 
These points adjacent to the edges are the feature points.
Since only first two steps are considered, so the two points adjacent to the first step, and the two adjacent to the second, 
will comprise a set of features for a single scan in the interest region. The points A and B show the location of the first 
and the second edges of the stairs.



# Future Improvements: 
* Use of a smaller depth camera like the Intel Realsense.
* Use of compact single board computer like the Odroid or Raspberri Pi or Nvidia Jetson etc.
* Use of Deep Learning framework.
* Integrate the camera to a some kind of eyerwear (like glasses), so that stairs are still visible if the user wears trousers.

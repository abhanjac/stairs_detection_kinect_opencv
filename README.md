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

# Algorithm Description:



# Future Improvements: 
* Use of a smaller depth camera like the Intel Realsense.
* Use of compact single board computer like the Odroid or Raspberri Pi or Nvidia Jetson etc.
* Use of Deep Learning framework.
* Integrate the camera to a some kind of eyerwear (like glasses), so that stairs are still visible if the user wears trousers.

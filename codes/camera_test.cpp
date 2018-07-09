// program to display the video from camera by opencv

#include <opencv2/opencv.hpp>
#include <iostream>
// namespace declaration
using namespace cv;
using namespace std;
// creating variables
VideoCapture vid;
Mat frame;

// main program
int main (int argc, char** argv)
{
  startWindowThread();
  // opening video file and storing in video variable
  vid.open( string(argv[1]) );
  // setting the capture device properties
  vid.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  vid.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  // creating the window to play the video
  namedWindow("Camera Window", CV_WINDOW_AUTOSIZE); // default window size
  // creating a while loop and displaying the video frame by frame till any keystroke
  while(1)
  {
    vid >> frame;
    if( !frame.data )
      break; // if there is no frame data, then break
    imshow("Camera Window", frame);
    cout << "code running" << endl;
//    if( waitKey(33) >= 0 )
//      break; // if the user presses any key then break
  }

  return 0;

}



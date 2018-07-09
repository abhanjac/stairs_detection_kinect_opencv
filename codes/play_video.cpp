// program to play a video by opencv

//#include <opencv2/opencv.hpp>
//#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
// namespace declaration
using namespace cv;
using namespace std;
// creating variables
VideoCapture vid;
Mat frame;

// main program
int main (int argc, char** argv)
{
  // opening video file and storing in video variable
  vid.open( string(argv[1]) );
  // creating the window to play the video
  namedWindow("Play Window", 0); // default window size
  // creating a while loop and displaying the video frame by frame till any keystroke
  while(1)
  {
    vid >> frame;
    if( !frame.data )
      break; // if there is no frame data, then break
    imshow("Play Window", frame);
    if( waitKey(33) >= 0 )
      break; // if the user presses any key then break
  }

  return 0;

}



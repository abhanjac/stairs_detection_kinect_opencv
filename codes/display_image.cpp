// program to display an image by opencv

//#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
// namespace declaration
using namespace cv;
using namespace std;
// creating variables
Mat image;

// main program
int main (int argc, char** argv)
{
  // opening image file and storing in image variable
  image = imread(argv[1]);
  // creating window and displaying the image till any keystroke
  namedWindow("Display Window", 0); // default window size
  imshow("Display Window", image);
  waitKey(0);

  return 0;

}



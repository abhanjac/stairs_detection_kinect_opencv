// program to display the video from kinect by opencv

#include <opencv2/opencv.hpp>
#include <iostream>

// namespace declaration
using namespace cv;
using namespace std;

// functions
void drawCenter(void* img1, Point2f center, Scalar s);

// main program
int main (int argc, char** argv)
{
  VideoCapture capture(CV_CAP_OPENNI);
  //VideoCapture capture(0);
  Mat bgr_image, depth_image, edge1_image, edge8_image;
  int i=0, ACTUAL_WIDTH, ACTUAL_HEIGHT, WIDTH = 320, HEIGHT = 240;
  double minDepth, maxDepth;
  char s[100], key = '/';

  // creating a while loop and displaying the video frame by frame till any keystroke
  while(1)
  {
    capture.grab();
    capture.retrieve( bgr_image, CV_CAP_OPENNI_BGR_IMAGE );
    capture.retrieve( depth_image, CV_CAP_OPENNI_DEPTH_MAP );
    ACTUAL_WIDTH = capture.get(CV_CAP_PROP_FRAME_WIDTH); // saving the actual width of frame
    ACTUAL_HEIGHT = capture.get(CV_CAP_PROP_FRAME_HEIGHT); // saving the actual height of frame

    edge1_image.create(bgr_image.size(), CV_8UC1); edge1_image = Scalar::all(0);
    edge8_image.create(bgr_image.size(), CV_8UC3); edge8_image = Scalar::all(0);

    cvtColor(bgr_image, edge1_image, CV_RGB2GRAY);

    minMaxIdx(depth_image, &minDepth, &maxDepth);
    depth_image.convertTo(edge8_image, CV_8UC3, 255/(maxDepth-minDepth), -minDepth*255/(maxDepth-minDepth));
    applyColorMap(edge8_image, edge8_image, COLORMAP_RAINBOW);

    drawCenter(&edge8_image, Point(ACTUAL_WIDTH/2, ACTUAL_HEIGHT/2), Scalar(0,0,0));
    drawCenter(&edge8_image, Point(ACTUAL_WIDTH/2, ACTUAL_HEIGHT/2), Scalar(0,0,0));
    namedWindow("BGR Window", 0); // default window size
    resizeWindow("BGR Window", WIDTH, HEIGHT); moveWindow("BGR Window", 600, 20);
    namedWindow("Depth Window", 0); // default window size
    resizeWindow("Depth Window", WIDTH, HEIGHT); moveWindow("Depth Window", 1050, 20);
    imshow("BGR Window", bgr_image);
    imshow("Depth Window", edge8_image);

    cout << (int)depth_image.at<uint16_t>(ACTUAL_HEIGHT/2, ACTUAL_WIDTH/2) << "   " << maxDepth << endl;

    key = waitKey(33);
    switch (key)
    {
      case 'q': break;
      default: break;
    }// switch
    if( key == 'q' )
      break; // if the user presses any key then break
  }

  return 0;

}




void drawCenter(void* img1, Point2f center, Scalar s)
{
  Mat img = *(Mat*)img1;
  circle(img, center, 2, s, 2); // marking the point
  circle(img, center, 7, s, 1);
}// drawCenter




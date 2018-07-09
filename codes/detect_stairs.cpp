// ----------------------------------------------------------------------------- //
// Detection of REAL and MODEL Stairs Moving Up and Down Using Image Processing. //
// Date:			December 2, 2016											 //
// ----------------------------------------------------------------------------- //

// Including header files and namespaces.
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string.h>
#include <math.h>
using namespace cv;
using namespace std;

// Declaring functions.
void drawCenter(Mat, Point2f, Scalar); // Function to draw dots or small circles on the image.

// Main program.
int main(int argc, char** argv)
{ // Defining objects.
  VideoCapture capture(CV_CAP_OPENNI); // Objects to capture the RGB and Depth data from the Kinect.
  // In OpenCV the sequence of primary colors are refered as Blue-Green-Red (BGR) sequence not as the usual RGB sequence.
  // Hence, we will also be following the same convention in all our comments.
  Mat BGRimgDn, DPTimgDn; // Matrix objects to hold the BGR and Depth frames from the Kinect video feed, for detecting Down stairs.
  Mat BGRimgUp, DPTimgUp; // Matrix objects to hold the BGR and Depth frames from the Kinect video feed, for detecting Up stairs.
  // The entire video frame is not used for our analysis. The useful portion is cropped and saved in the following Matrix objects.
  Mat cropBGRimgDn, cropDPTimgDn; // Matrix objects to hold the cropped BGR and Depth frames from the Kinect, for detecting Down stairs.
  Mat cropBGRimgUp, cropDPTimgUp; // Matrix objects to hold the cropped BGR and Depth frames from the Kinect, for detecting Down stairs.
  int ACTUAL_WIDTH = 640, ACTUAL_HEIGHT = 480; // Actual height and width of the Kinect video frames (BGR and Depth frames are of same size).
  int WIDTH = 320, HEIGHT = 240; // This is the height and the width of the windows that shows the BGR and Depth video feeds.
  int MODEL = 1; // MODEL = 0 indicates that the program will detect REAL stairs, MODEL = 1 means it will detect model stairs (by default it is 1).
  int saveIdx = 0; // Index of the saved image.
  double BGR_VER_FOV = 62, BGR_HOR_FOV = 48.6; // Vertical and horizontal Field Of View of the BGR camera of the Kinect.
  double DPT_VER_FOV = 58.5, DPT_HOR_FOV = 46.6; // Vertical and horizontal Field Of View of the Depth camera of the Kinect.
  double minDepth = 0.0, maxDepth = 0.0; // Objects to  hold the minimum and maximum depth values of the Depth image from the Kinect.
  char key = '`', outputMsg[100]; // String to display a text on the screen.

  while(1)
  {
    // COPYING THE IMAGE FROM SOURCE.
    capture.grab(); // Capturing the BGR and Depth frames from the kinect.
	// The BGR and Depth frame has to be stored in some matrix object for our analysis.
    // For convenience, we are copying the same BGR frame into two separate matrices, one of which will be used for analysis of Up stairs,
    // and the other for Down stairs. Same thing is done for the Depth frame as well.
    capture.retrieve( BGRimgDn, CV_CAP_OPENNI_BGR_IMAGE ); // Copying the BGR frame into the BGRimgDn matrix, for analysis of Down stairs.
	capture.retrieve( DPTimgDn, CV_CAP_OPENNI_DEPTH_MAP ); // Copying the Depth frame into the DPTimgDn matrix, for analysis of Down stairs.
	capture.retrieve( BGRimgUp, CV_CAP_OPENNI_BGR_IMAGE ); // Copying the BGR frame into the BGRimgDn matrix, for analysis of Up stairs.
	capture.retrieve( DPTimgUp, CV_CAP_OPENNI_DEPTH_MAP ); // Copying the Depth frame into the DPTimgDn matrix, for analysis of Up stairs.

    if( !BGRimgDn.data || !DPTimgDn.data )
      break; // If no valid data is there in the matrices, then break the loop and terminate the program.

    ACTUAL_WIDTH = BGRimgDn.cols;  ACTUAL_HEIGHT = BGRimgDn.rows; // Calculating actual height and width of the images.

    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ //
    // ---- PROCESSING FOR DOWN STAIRS ------------------------------------------------------------------------------------------------- //
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ //

    /* --------------------------------------- */
    /* DEFINING THE PARAMETERS FOR DOWN STAIRS */
    /* --------------------------------------- */

    // By default the program is in "MODEL" stairs detection mode.
    int widthPercentDn = 20, heightPercentDn = 50; // Percentage of actual image that will be considered as interest region for down stairs.
    int minSenseDepthDn = 400, maxSenseDepthDn = 1200;
    double mountAngleDn = 42.0, mountHeightDn = 485; // Kinect mount angle with the horizontal and the mount height for the ground.
    double TH00 = 164.2443, TH10 = -0.2036, TH20 = 1.0059, estDepP1HgOffSetDn = 20, estDepP1LwOffSetDn = -20;
    double TH01 = 184.0495, TH11 = 0.0413, TH21 = 0.9777, estDepP2HgOffSetDn = 20, estDepP2LwOffSetDn = -20;
    double TH02 = -47.0351, TH12 = -0.0163, TH22 = 0.7540, TH32 = 0.075, estDisP2HgOffSetDn = 20, estDisP2LwOffSetDn = -20;
    double TH03 = 13.7039, TH13 = -0.0537, TH23 = 0.082, TH33 = 0.9937, estDepAvHgOffSetDn = 20, estDepAvLwOffSetDn = -20;
    double flag0Dn = 0, flag1Dn = 0, flag2Dn = 0, flag3Dn = 0;

    if( MODEL == 0 ) // Program is in "REAL" stairs detection mode when the MODEL flag is made 0.
    {
      widthPercentDn = 50, heightPercentDn = 40; // Percentage of actual image that will be considered as interest region for down stairs.
      minSenseDepthDn = 800, maxSenseDepthDn = 2100;
      mountAngleDn = 45.0, mountHeightDn = 880; // Kinect mount angle with the horizontal and the mount height for the ground.
      TH00 = 252.523, TH10 = -0.3195, TH20 = 0.9829, estDepP1HgOffSetDn = 50, estDepP1LwOffSetDn = -20;
      TH01 = 334.1068, TH11 = -0.0223, TH21 = 0.985, estDepP2HgOffSetDn = 30, estDepP2LwOffSetDn = -20;
      TH02 = -85.0389, TH12 = -0.0056, TH22 = 0.7835, TH32 = 0.041, estDisP2HgOffSetDn = 30, estDisP2LwOffSetDn = -20;
      TH03 = 40.4137, TH13 = -0.0064, TH23 = 0.1426, TH33 = 1.001, estDepAvHgOffSetDn = 20, estDepAvLwOffSetDn = -30;
      flag0Dn = 0, flag1Dn = 0, flag2Dn = 0, flag3Dn = 0;
    }// if.

    /* ----------------------------------------------------- */
    /* EXTRACTING THE FEATURES FROM THE DATA FOR DOWN STAIRS */
    /* ----------------------------------------------------- */

    // MASKING OUT THE UNWANTED PART OF THE IMAGE THAT IS SEEN IN THE UPPER PART OF THE DEPTH IMAGE.
    int intRegWidthDn = ACTUAL_WIDTH*widthPercentDn/100, intRegHeightDn = ACTUAL_HEIGHT*heightPercentDn/100;
    int leftDn = ACTUAL_WIDTH*(100 - widthPercentDn)*0.5/100, topDn = ACTUAL_HEIGHT*(100 - heightPercentDn)/100; // Finding the top left vertex.
    Rect rDn(leftDn, topDn, intRegWidthDn, intRegHeightDn); // Rectangular boundary around interest region along which image will be cropped.

    // Cropping down stair image in the interest region and storing them in a separate set.
    BGRimgDn(rDn).copyTo(cropBGRimgDn);  DPTimgDn(rDn).copyTo(cropDPTimgDn);
  
    // RUNNING MULTIPLE SCANS ON THE IMAGE FROM TOP TO BOTTOM AND RECORDING THE DEPTH VARIATION ALONG EACH SCAN.
    int scanSpacingDn = 10, scanStepDn = 5, scanStepBetwDn = 2;
    vector < vector<Rect> > scanPtsDPTimgDn, scanPtsDPTfullImgDn;

    for( int c = 0; c < intRegWidthDn; c+= scanSpacingDn )
    { // scanSpacingDn is the spacing between adjascent scan lines.
      vector<Rect> dptTemp; // Temporary vector to hold one scan point along the current scan line.
      int flag1 = 0, flag2 = 0, flag3 = 0; // Flags to indicate that the spikes signifying the steps edges have been detected.
      double avgDepthBetwStepDn = 0.0; // We will also calculate the average depth between the spikes.
      Rect rpt1, rpt2, rpt3, rpt4; // The points are stored as rectangle datatypes.
      for( int r = intRegHeightDn-1-scanStepDn; r > -1; r-= scanStepDn )
      { // scanStepDn is the gap between successive points on same scan line.
        int depth1 = cropDPTimgDn.at<uint16_t>(r,c), depth2 = cropDPTimgDn.at<uint16_t>(r+scanStepDn,c);
        if( (depth1 - depth2 > 100) && (flag1 == 0) ) // Storing consecutive points between which depth is changing abruptly, first spike.
        if( (depth1 > minSenseDepthDn) && (depth1 < maxSenseDepthDn) ) // The depth of the points should be between some threshold.
        if( (depth2 > minSenseDepthDn) && (depth2 < maxSenseDepthDn) )
        {
          rpt1 = Rect(c,r+scanStepDn,depth2,0);          rpt2 = Rect(c,r,depth1,0);          flag1++;          continue;
        }// if.
        if( (depth1 - depth2 > 100) && (flag2 == 0) ) // Storing consecutive points between which depth is changing abruptly, second spike.
        if( (depth1 > minSenseDepthDn) && (depth1 < maxSenseDepthDn) ) // The depth of the points should be between some threshold.
        if( (depth2 > minSenseDepthDn) && (depth2 < maxSenseDepthDn) )
        {
          rpt3 = Rect(c,r+scanStepDn,depth2,0);          rpt4 = Rect(c,r,depth1,0);          flag2++;
        }// if.
        if( flag1 == 1 && flag2 == 1 )
        { // This means we have got four points.
          int itr = 0; // Calculate the no. of steps taken to find the average depth between point 2 and 3.
          for( int n = rpt2.y; n >= rpt3.y; n -= scanStepBetwDn )
          { // Taking the average of depths from point 2 to 3.
            avgDepthBetwStepDn += cropDPTimgDn.at<uint16_t>(n,c); // Taking the sum of the depths.
            itr++;
          }// for.
          if( itr > 0 ) // This means there is atleast one sample for the depth between the two spikes.
            avgDepthBetwStepDn /= itr; // Taking the average.
          if( (avgDepthBetwStepDn > minSenseDepthDn) && (avgDepthBetwStepDn < maxSenseDepthDn) )
          { // Taking samples whose average depth is in this range and Storing them inside the rect variables.
            rpt1.height = (int)avgDepthBetwStepDn;            rpt2.height = (int)avgDepthBetwStepDn;
            rpt3.height = (int)avgDepthBetwStepDn;            rpt4.height = (int)avgDepthBetwStepDn;
            flag3 = 1;            break;
          }// if.
        }// if.
      }// for.
      if( flag1 == 1 && flag2 == 1 && flag3 == 1 )
      { // This means we have got all the information needed.
        dptTemp.push_back(rpt1);        dptTemp.push_back(rpt2);        dptTemp.push_back(rpt3);        dptTemp.push_back(rpt4);
        scanPtsDPTimgDn.push_back(dptTemp); // Storing complete vectors of the four scanned points into a larger vector.

        // Storing the same data in another vector adding the offset of the cropped image margin, for displaying in the full image.
        vector<Rect> dptTempFullImg;
        dptTempFullImg.push_back( Rect( rpt1.x+leftDn, rpt1.y+topDn, rpt1.width, rpt1.height ) );
        dptTempFullImg.push_back( Rect( rpt2.x+leftDn, rpt2.y+topDn, rpt2.width, rpt2.height ) );
        dptTempFullImg.push_back( Rect( rpt3.x+leftDn, rpt3.y+topDn, rpt3.width, rpt3.height ) );
        dptTempFullImg.push_back( Rect( rpt4.x+leftDn, rpt4.y+topDn, rpt4.width, rpt4.height ) );
        scanPtsDPTfullImgDn.push_back(dptTempFullImg); // This is the stored vector relative to the full image.
      }// if.
    }// for.

    /* ------------------------------------------------------ */
    /* DISPLAYING THE DOWN STAIR IMAGE AND THE FEATURE POINTS */
    /* ------------------------------------------------------ */
	
    Mat temp50, temp100, temp110, temp60, temp120, temp130;
    cropBGRimgDn.copyTo(temp50);    cropDPTimgDn.copyTo(temp100);    minMaxIdx(temp100, &minDepth, &maxDepth);
    temp100.convertTo(temp110, CV_8UC3, 255/(maxDepth-minDepth), -minDepth);    applyColorMap(temp110, temp110, COLORMAP_RAINBOW);
    BGRimgDn.copyTo(temp60);    DPTimgDn.copyTo(temp120);    minMaxIdx(temp120, &minDepth, &maxDepth);
    temp120.convertTo(temp130, CV_8UC3, 255/(maxDepth-minDepth), -minDepth);    applyColorMap(temp130, temp130, COLORMAP_RAINBOW);

    for( int i = 0; i < scanPtsDPTimgDn.size(); i++ ) // Marking all the points that are recorded for analysis.
    {
      for( int j = 0; j < scanPtsDPTimgDn[i].size(); j++ )
      { // Displaying in the cropped image and the full image.
        Point2f pc( scanPtsDPTimgDn[i][j].x, scanPtsDPTimgDn[i][j].y );        drawCenter(temp110, pc, Scalar(0,0,0));
        Point2f pf( scanPtsDPTfullImgDn[i][j].x, scanPtsDPTfullImgDn[i][j].y );        drawCenter(temp130, pf, Scalar(0,0,0));
      }// for.
      Point2f pc( scanPtsDPTimgDn[i][0].x, (scanPtsDPTimgDn[i][1].y + scanPtsDPTimgDn[i][2].y)/2 );
      drawCenter(temp110, pc, Scalar(0,0,0)); // Drawing average depth point midway between 2nd (1th) and 3rd (2th) point in cropped image.
      Point2f pf( scanPtsDPTfullImgDn[i][0].x, (scanPtsDPTfullImgDn[i][1].y + scanPtsDPTfullImgDn[i][2].y)/2 );
      drawCenter(temp130, pf, Scalar(0,0,0)); // Drawing average depth point midway between 2nd (1th) and 3rd (2th) point in full image.
    }// for.

    /* --------------------------------------------------------------------------------------------------------------------------- */
    /* SETTING UP THE PARAMETERS AND DECIDING IF THIS STAIR IS MOVING DOWN AND MARKING THE FEATURE POINTS ON THE DOWN STAIRS IMAGE */
    /* --------------------------------------------------------------------------------------------------------------------------- */

    vector < vector<Rect> > scanPtsFINALdn, scanPtsFINALfullImgDn; // Vectors to store the final selected points.

    // CHECKING IF THE RECORDED DATA FALLS WITHIN THE PREDEFINED THRESHOLD OF THE PARAMETERIZED MODEL OF THE DOWN STAIRS.
    for( int i = 0; i<scanPtsDPTimgDn.size(); i++ )
    {
      // Calculating the estimated depths of the points.
      double estDepP1Dn = TH00 + TH10 * scanPtsDPTimgDn[i][0].y + TH20 * scanPtsDPTimgDn[i][0].width;
      double estDepP2Dn = TH01 + TH11 * scanPtsDPTimgDn[i][0].y + TH21 * scanPtsDPTimgDn[i][0].width;
      double estDisP2Dn = TH02 + TH12 * scanPtsDPTimgDn[i][0].x + TH22 * scanPtsDPTimgDn[i][0].y + TH32 * scanPtsDPTimgDn[i][0].width;
      double estDepAvDn = TH03 + TH13 * scanPtsDPTimgDn[i][1].x + TH23 * scanPtsDPTimgDn[i][1].y + TH33 * scanPtsDPTimgDn[i][1].width;

      // Checking if the actual depth is within the accepted threshold offsets from the estimated depth, if so then raise a flag.
      if( scanPtsDPTimgDn[i][1].width < estDepP1Dn + estDepP1HgOffSetDn && scanPtsDPTimgDn[i][1].width > estDepP1Dn + estDepP1LwOffSetDn )
        flag0Dn = 1;
      if( scanPtsDPTimgDn[i][2].width < estDepP2Dn + estDepP2HgOffSetDn && scanPtsDPTimgDn[i][2].width > estDepP2Dn + estDepP2LwOffSetDn )
        flag1Dn = 1;
      if( scanPtsDPTimgDn[i][2].y < estDisP2Dn + estDisP2HgOffSetDn && scanPtsDPTimgDn[i][2].y > estDisP2Dn + estDisP2LwOffSetDn )
        flag2Dn = 1;
      if( scanPtsDPTimgDn[i][1].height < estDepAvDn + estDepAvHgOffSetDn && scanPtsDPTimgDn[i][1].height > estDepAvDn + estDepAvLwOffSetDn )
        flag3Dn = 1;

      // IF ALL THE CRITERIA ARE SATIFIED THEN DISPLAYING THE FEATURE POINTS IN THE IMAGE
      if( flag0Dn > 0 && flag1Dn > 0 && flag2Dn > 0 && flag3Dn > 0 ) // This means all the conditions are satisfied.
      { // The set of scanned points that staisfies the parameterized model are stored in a separate vector.
        scanPtsFINALdn.push_back( scanPtsDPTimgDn[i] ); // This is relative to cropped image.
        scanPtsFINALfullImgDn.push_back( scanPtsDPTfullImgDn[i] ); // This is relative to full image.
      }// if.
      else
        cout << " " << flag0Dn << flag1Dn << flag2Dn << flag3Dn << " " << endl;
        ;
    }// for.

    // CALCULATING THE DISTANCE FORM THE DOWN STAIRS AND DISPLAYING THEM.
    // Parameters to calculate average distance of the person from stairs. Distances are in mm and angles are in degrees.
    double distanceDn = 0.0, qualifyPercentDn = 0.75;
    int dnSampleCount = 0; // Flag to indicate that stairs moving down has been detected and also how many such scan samples are detected.
    
    if( scanPtsFINALdn.size() >= qualifyPercentDn * scanPtsDPTimgDn.size() )
    { // Only if the 'qualifyPercentDn' percent of the scanned points satisfy the parameterized model, then display that stairs are detected.
      for( int i = 0; i<scanPtsFINALdn.size(); i++ )
      {
        for( int j = 0; j < scanPtsFINALdn[i].size(); j++ )
        { // Displaying in the cropped image and the full image.
          Point2f pc( scanPtsFINALdn[i][j].x, scanPtsFINALdn[i][j].y );        drawCenter(temp50, pc, Scalar(0,255,255));
          Point2f pf( scanPtsFINALfullImgDn[i][j].x, scanPtsFINALfullImgDn[i][j].y );        drawCenter(temp60, pf, Scalar(0,255,255));
        }// for.
        Point2f pc( scanPtsFINALdn[i][0].x, (scanPtsFINALdn[i][1].y + scanPtsFINALdn[i][2].y)/2 );
        drawCenter(temp50, pc, Scalar(0,255,255)); // Drawing average depth point midway between 2nd (1th) and 3rd (2th) point in cropped image.
        Point2f pf( scanPtsFINALfullImgDn[i][0].x, (scanPtsFINALfullImgDn[i][1].y + scanPtsFINALfullImgDn[i][2].y)/2 );
        drawCenter(temp60, pf, Scalar(0,255,255)); // Drawing average depth point midway between 2nd (1th) and 3rd (2th) point in full image.

        // Calculating view angles from field of view (FOV).
        double verViewAngle = ( scanPtsFINALfullImgDn[i][0].y - ACTUAL_HEIGHT/2 ) * DPT_VER_FOV / ACTUAL_HEIGHT ; // Vertical view angle.
        double horViewAngle = ( scanPtsFINALfullImgDn[i][0].x - ACTUAL_WIDTH/2 ) * DPT_HOR_FOV / ACTUAL_WIDTH ; // Horizontal view angle.
        // The very first edge of the stairs is used to calculate the distance. When we are climbimg down the stairs, at every step the 1st 
        // edge of the next step is used to measure the distance.
        distanceDn += scanPtsFINALfullImgDn[i][0].width * cos( (verViewAngle + mountAngleDn)*CV_PI/180 ) * cos( horViewAngle*CV_PI/180 );
        dnSampleCount++; // Counting how many samples are being considered as detected stairs.
      }// for.

      // DISPLAYING THE DISTANCE OF DOWN STAIRS.
      distanceDn /= dnSampleCount; // Calculating the average distance.
      if( (distanceDn > 100) && (distanceDn < 1000) )
      { // If the distance is between 100mm and 1000mm, then calculate in cm.
        sprintf(outputMsg, "Stairs Detected at %0.2f cm (DOWN)", 0.1*distanceDn);
        putText(temp60, outputMsg, Point( 20, 40 ), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(255,0,0), 4); // Writing a text.
      }// if.
      if( distanceDn > 1000 )
      { // If the distance is greater than 1000mm, then calculate in m.
        sprintf(outputMsg, "Stairs Detected at %0.2f m (DOWN)", 0.001*distanceDn);
        putText(temp60, outputMsg, Point( 20, 40 ), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(255,0,0), 4); // Writing a text.
      }// if.
    }// if.

    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ //
    // ---- PROCESSING FOR UP STAIRS --------------------------------------------------------------------------------------------------- //
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ //

    /* ------------------------------------- */
    /* DEFINING THE PARAMETERS FOR UP STAIRS */
    /* ------------------------------------- */

    // By default the program is in "MODEL" stairs detection mode.
    int widthPercentUp = 25, heightPercentUp = 100; // Percentage of actual image that will be considered as interest region for up stairs.
    int minSenseDepthUp = 400, maxSenseDepthUp = 800;
    double mountAngleUp = 42.0, mountHeightUp = 485; // Kinect mount angle with the horizontal and the mount height for the ground.
    double PH00 = -97.3592, PH10 = 0.0585, PH20 = 0.9768, estDepP1HgOffSetUp = 30, estDepP1LwOffSetUp = -20;
    double PH01 = -36.7519, PH11 = 0.0494, PH21 = 1.0065, estDepP2HgOffSetUp = 20, estDepP2LwOffSetUp = -20;
    double PH02 = -349.7489, PH12 = -0.0112, PH22 = 1.0507, PH32 = 0.2898, estDisP2HgOffSetUp = 30, estDisP2LwOffSetUp = -20;
    double PH03 = 28.0292, PH13 = -0.0081, PH23 = -0.0013, PH33 = 1.0301, estDepAvHgOffSetUp = 20, estDepAvLwOffSetUp = -30;
    double flag0Up = 0, flag1Up = 0, flag2Up = 0, flag3Up = 0, flag4Up = 1, flag5Up = 1;

    if( MODEL == 0 ) // Program is in "REAL" stairs detection mode when the MODEL flag is made 0.
    {
      widthPercentUp = 50, heightPercentUp = 100; // Percentage of actual image that will be considered as interest region for up stairs.
      minSenseDepthUp = 400, maxSenseDepthUp = 1400;
      mountAngleUp = 45.0, mountHeightUp = 880; // Kinect mount angle with the horizontal and the mount height for the ground.
      PH00 = -220.2735, PH10 = 0.0945, PH20 = 1.0829, estDepP1HgOffSetUp = 60, estDepP1LwOffSetUp = -20;
      PH01 = -5.9768, PH11 = 0.0802, PH21 = 1.0612, estDepP2HgOffSetUp = 110, estDepP2LwOffSetUp = -20;
      PH02 = -351.6235, PH12 = 0.0031, PH22 = 0.9932, PH32 = 0.1894, estDisP2HgOffSetUp = 30, estDisP2LwOffSetUp = -20;
      PH03 = 118.7745, PH13 = -0.0120, PH23 = 0.0253, PH33 = 1.0189, estDepAvHgOffSetUp = 100, estDepAvLwOffSetUp = -30;
      flag0Up = 0, flag1Up = 0, flag2Up = 0, flag3Up = 0, flag4Up = 0, flag5Up = 0;
    }// if.

    /* --------------------------------------------------- */
    /* EXTRACTING THE FEATURES FROM THE DATA FOR UP STAIRS */
    /* --------------------------------------------------- */

    // MASKING OUT THE UNWANTED PART OF THE IMAGE THAT IS SEEN IN THE UPPER PART OF THE DEPTH IMAGE.
    int intRegWidthUp = ACTUAL_WIDTH*widthPercentUp/100, intRegHeightUp = ACTUAL_HEIGHT*heightPercentUp/100;
    int leftUp = ACTUAL_WIDTH*(100 - widthPercentUp)*0.5/100, topUp = ACTUAL_HEIGHT*(100 - heightPercentUp)/100; // Finding the top left vertex.
    Rect rUp(leftUp, topUp, intRegWidthUp, intRegHeightUp); // Rectangular boundary around interest region along which image will be cropped.

    // Cropping up stair image in the interest region and storing them in a separate set.
    BGRimgUp(rUp).copyTo(cropBGRimgUp);  DPTimgUp(rUp).copyTo(cropDPTimgUp);
  
    // RUNNING MULTIPLE SCANS ON THE IMAGE FROM TOP TO BOTTOM AND RECORDING THE DEPTH VARIATION ALONG EACH SCAN.
    int scanSpacingUp = 10, scanStepUp = 3, scanStepBetwUp = 2;
    vector < vector<Vec4f> > scanPtsDPTimgUp, scanPtsDPTfullImgUp;
    // These vectors hold actual x, y and actual depth values of the scans.

    for( int c = 0; c < intRegWidthUp; c+= scanSpacingUp )
    { // scanSpacingUp is the spacing between adjascent scan lines.
      vector<Vec4f> dptTemp, dptTempFullImg; // Temporary vector to hold one scan point along the current scan line.
      double avgDepthBetwStepUp = 0.0; // We will also calculate the average depth between the spikes.
      for( int r = 0; r < intRegHeightUp; r+= scanStepUp )
      { // scanStepUp is the gap between successive points on same scan line.
        float depth = cropDPTimgUp.at<uint16_t>(r,c);
        // Storing all points from top to bottom margin of the interest region.
        if( (depth > minSenseDepthUp) && (depth < maxSenseDepthUp) )
        { // The depth of the points should be between some threshold.
          dptTemp.push_back( Vec4f( c, r, depth, 0 ) ); // Storing all the scanned points into a larger vector for the cropped image.
          // Storing all the scanned points into another larger vector for full image.
          dptTempFullImg.push_back( Vec4f( c+leftUp, r+topUp, depth, 0 ) );
        }// if.
      }// for.
      if( dptTemp.size() > 0 ) // Only if we have some valid scanned points, they are stored in a separate vector.
      {
        scanPtsDPTimgUp.push_back(dptTemp); // Storing all the scanned points into a larger vector, this vector is relative to the cropped image.
        scanPtsDPTfullImgUp.push_back(dptTempFullImg); // This is the stored vector relative to the full image.
      }// if.
    }// for.

    // TAKING THE MOVING AVERAGE OF THE SCANNED POINTS FOR MAKING THE SCANNED PATTERN SMOOTHER.
    int itrUp = 8; // itrUp is the number of samples considered for moving average.
    vector < vector<Vec4f> > mvAvgScanDPTimgUp, mvAvgScanDPTfullImgUp;
    // These vectors hold actual x, y, average depth and derivative of depth values of the scans.

    for( int c = 0; c < scanPtsDPTimgUp.size(); c++ )
    { // Taking the moving average along the columns of the scanned points.
      vector<Vec4f> dptTemp, dptTempFullImg;
      for( int i = 0; i < itrUp/2; i++ )
      { // Storing first itrUp/2 samples of original scanned points in front of moving average values.
        dptTemp.push_back(scanPtsDPTimgUp[c][i]);
        dptTempFullImg.push_back(scanPtsDPTfullImgUp[c][i]);
      }// for.

      for( int i = itrUp; i < scanPtsDPTimgUp[c].size(); i++ )
      { // Storing the moving average values.
        float avgVal = 0;
        for( int j = 0; j < itrUp; j++ ) // Calculating sum of depth values of scanned points of previous itrUp number of samples.
          avgVal += scanPtsDPTimgUp[c][i-j][2];
        avgVal /= itrUp; // Calculating the average.
        float x, y; // The moving average will cause the depth vs y waveform to shift, hence these offsets in the index is added.
        x = scanPtsDPTimgUp[c][ i - ((itrUp+1)/2) ][0];        y = scanPtsDPTimgUp[c][ i - ((itrUp+1)/2) ][1];
        dptTemp.push_back( Vec4f( x, y, avgVal, 0 ) );
        x = scanPtsDPTfullImgUp[c][ i - ((itrUp+1)/2) ][0];        y = scanPtsDPTfullImgUp[c][ i - ((itrUp+1)/2) ][1];
        dptTempFullImg.push_back( Vec4f( x, y, avgVal, 0 ) );
      }// for.

      for( int i = scanPtsDPTimgUp[c].size() - (itrUp+1)/2; i < scanPtsDPTimgUp[c].size(); i++ )
      { // Storing last itrUp/2 samples of the original scanned points at the end of the moving average values.
        dptTemp.push_back(scanPtsDPTimgUp[c][i]);
        dptTempFullImg.push_back(scanPtsDPTfullImgUp[c][i]);
      }// for.
      mvAvgScanDPTimgUp.push_back(dptTemp); // Storing all average values into a larger vector, this vector is relative to the cropped image.
      mvAvgScanDPTfullImgUp.push_back(dptTempFullImg); // This is the stored vector relative to the full image.
    }// for.

    // TAKING THE DERIVATIVE OF THE MOVING AVERAGE VALUES.
    for( int c = 0; c < mvAvgScanDPTimgUp.size(); c++ ) // Taking the derivative along the columns of the moving average of scanned points.
    { // The derivative of the first sample is taken using the first (0th element) and the next (second or 1st element) value.
      float n0 = mvAvgScanDPTimgUp[c][1][2] - mvAvgScanDPTimgUp[c][0][2];
      float d0 = mvAvgScanDPTimgUp[c][1][1] - mvAvgScanDPTimgUp[c][0][1];
      mvAvgScanDPTimgUp[c][0][3] = n0/d0;
      float n0Full = mvAvgScanDPTfullImgUp[c][1][2] - mvAvgScanDPTfullImgUp[c][0][2];
      float d0Full = mvAvgScanDPTfullImgUp[c][1][1] - mvAvgScanDPTfullImgUp[c][0][1];
      mvAvgScanDPTfullImgUp[c][0][3] = n0Full/d0Full;

      for( int i = 1; i < mvAvgScanDPTimgUp[c].size() - 1; i++ )
      { // Taking the derivative by using the previous and the next sample along the columns of the moving average of scanned points.
        float num = mvAvgScanDPTimgUp[c][i+1][2] - mvAvgScanDPTimgUp[c][i-1][2];
        float den = mvAvgScanDPTimgUp[c][i+1][1] - mvAvgScanDPTimgUp[c][i-1][1];
        mvAvgScanDPTimgUp[c][i][3] = num/den;
        float numFull = mvAvgScanDPTfullImgUp[c][i+1][2] - mvAvgScanDPTfullImgUp[c][i-1][2];
        float denFull = mvAvgScanDPTfullImgUp[c][i+1][1] - mvAvgScanDPTfullImgUp[c][i-1][1];
        mvAvgScanDPTfullImgUp[c][i][3] = numFull/denFull;
      }// for.

      int s = mvAvgScanDPTimgUp[c].size(); // Number of elements in each column of the moving average vector.
      // The derivative of the last sample is taken using the last (nth element) and the previous (second last or (n-1)th element) value.
      float nl = mvAvgScanDPTimgUp[c][s-1][2] - mvAvgScanDPTimgUp[c][s-2][2];
      float dl = mvAvgScanDPTimgUp[c][s-1][1] - mvAvgScanDPTimgUp[c][s-2][1];
      mvAvgScanDPTimgUp[c][s-1][3] = nl/dl;
      float nlFull = mvAvgScanDPTfullImgUp[c][s-1][2] - mvAvgScanDPTfullImgUp[c][s-2][2];
      float dlFull = mvAvgScanDPTfullImgUp[c][s-1][1] - mvAvgScanDPTfullImgUp[c][s-2][1];
      mvAvgScanDPTfullImgUp[c][s-1][3] = nlFull/dlFull;
    }// for.

    // TAKING THE MOVING AVERAGE OF THE DERIVATIVE VALUES FOR MAKING THE DERIVATIVE PATTERN SMOOTHER.
    int itrDeriUp = 8; // itrDeriUp is the number of samples considered for moving average of the derivative.
    vector < vector<Vec4f> > mvAvgDeriDPTimgUp, mvAvgDeriDPTfullImgUp;
    // These vectors hold actual x, y, average depth and average of the derivative of depth values of the scans.

    for( int c = 0; c < mvAvgScanDPTimgUp.size(); c++ )
    { // Taking the moving average along the columns of the derivative vector.
      vector<Vec4f> dptTemp, dptTempFullImg;
      for( int i = 0; i < itrDeriUp/2; i++ )
      { // Storing first itrDerUp/2 samples of original derivative of scanned points in front of moving average values.
        dptTemp.push_back(mvAvgScanDPTimgUp[c][i]);
        dptTempFullImg.push_back(mvAvgScanDPTfullImgUp[c][i]);
      }// for.

      for( int i = itrDeriUp; i < mvAvgScanDPTimgUp[c].size(); i++ )
      { // Storing the moving average values.
        float avgVal = 0;
        for( int j = 0; j < itrDeriUp; j++ ) // Calculating sum of depth derivatives of scanned points of previous itrDeriUp number of samples.
          avgVal += mvAvgScanDPTimgUp[c][i-j][3];
        avgVal /= itrDeriUp; // Calculating the average.
        float x, y, depth; // The moving average will cause the depth vs y waveform to shift, hence these offsets in the index is added.
        x = mvAvgScanDPTimgUp[c][ i - ((itrDeriUp+1)/2) ][0];        y = mvAvgScanDPTimgUp[c][ i - ((itrDeriUp+1)/2) ][1];
        depth = mvAvgScanDPTimgUp[c][ i - ((itrDeriUp+1)/2) ][2];        dptTemp.push_back( Vec4f( x, y, depth, avgVal ) );
        x = mvAvgScanDPTfullImgUp[c][ i - ((itrDeriUp+1)/2) ][0];        y = mvAvgScanDPTfullImgUp[c][ i - ((itrDeriUp+1)/2) ][1];
        depth = mvAvgScanDPTfullImgUp[c][ i - ((itrDeriUp+1)/2) ][2];        dptTempFullImg.push_back( Vec4f( x, y, depth, avgVal ) );
      }// for.

      for( int i = mvAvgScanDPTimgUp[c].size() - (itrDeriUp+1)/2; i < mvAvgScanDPTimgUp[c].size(); i++ )
      { // Storing last itrDeriUp/2 samples of the original derivative of scanned points at the end of the moving average values.
        dptTemp.push_back( mvAvgScanDPTimgUp[c][i] );
        dptTempFullImg.push_back( mvAvgScanDPTfullImgUp[c][i] );
      }// for.
      mvAvgDeriDPTimgUp.push_back( dptTemp ); // Storing all average values into a larger vector, this vector is relative to the cropped image.
      mvAvgDeriDPTfullImgUp.push_back( dptTempFullImg ); // This is the stored vector relative to the full image.
    }// for.

    // FINDING THE POINTS WHERE THE DERIVATIVE OR GRADIENT OF THE DEPTH BECOMES 0 OR CHANGES SIGN.
    vector < vector<Vec4f> > deriZeroDPTimgUp, deriZeroDPTfullImgUp;
    // These vectors hold actual x, y, average depth and average depth between first minima and second maxima of depth values of the scans.

    for( int c = 0; c < mvAvgDeriDPTimgUp.size(); c++ )
    { // Finding the first two maxima and minima.
      int flag0 = 0, flag1 = 0, i0 = 0, i1 = 0;
      vector<Vec4f> dptTemp, dptTempFullImg;
      for( int i = mvAvgDeriDPTimgUp[c].size() - 1; i > 0; i-- )
      { // Looking through the scanned point columns from the bottom of the image to the top.
        if( mvAvgDeriDPTimgUp[c][i][3] < 0 && mvAvgDeriDPTimgUp[c][i-1][3] > 0 )
        if( ( flag0 == 0 && flag1 == 0 ) || ( flag0 == 1 && flag1 == 1 ) ) // Finding maxima.
        { // Only the first two maxima and the first minima will is required for our analysis.
          dptTemp.push_back( mvAvgDeriDPTimgUp[c][i] );          dptTempFullImg.push_back( mvAvgDeriDPTfullImgUp[c][i] );
          flag0++; // Once this flag0 becomes 2, this indicates that two maxima has been found and this 'if' will no longer be executed.
        }// if.
        if( flag0 == 2 && flag1 == 1 )
          i1 = i; // Recording the index when the second maxima is found.

        if( mvAvgDeriDPTimgUp[c][i][3] > 0 && mvAvgDeriDPTimgUp[c][i-1][3] < 0 )
        if( ( flag0 == 1 && flag1 == 0 ) || ( flag0 == 2 && flag1 == 1 ) ) // Finding minima.
        { // Only the first two maxima and the first minima will is required for our analysis.
          dptTemp.push_back( mvAvgDeriDPTimgUp[c][i] );          dptTempFullImg.push_back( mvAvgDeriDPTfullImgUp[c][i] );
          flag1++; // Once this flag1 becomes 2, this indicates that two minima has been found and this 'if' will no longer be executed.
        }// if.
        if( flag0 == 1 && flag1 == 1 )
          i0 = i; // Recording the index when the first minima is found.

        if( flag0 == 2 && flag1 == 2 )
        { // This means the two maxima and the minima has been found. So breaking the loop.
          float avgVal = 0;          int sampleCount = 0;
          for( int j = i0; j >= i1; j-- ) // Finding average depth between first minima and second maxima point.
          {
            avgVal += mvAvgDeriDPTimgUp[c][j][2]; // Calculating sum of all depth values between first minima and second maxima point.
            sampleCount++; // Counting the number of samples.
          }// for.
          avgVal /= sampleCount; // Calculating average depth between first minima and second maxima point.
          for( int j = 0; j < dptTemp.size(); j++ )
          {
            dptTemp[j][3] = avgVal; // Storing the average depth value in last element of each of the selected maxima and minima points.
            dptTempFullImg[j][3] = avgVal;
          }// for.

          deriZeroDPTimgUp.push_back( dptTemp ); // The final vector relative to cropped image.
          deriZeroDPTfullImgUp.push_back( dptTempFullImg ); // The final vector relative to full image.
          break;
        }// if.
      }// for.
    }// for.

    /* ---------------------------------------------------- */
    /* DISPLAYING THE UP STAIR IMAGE AND THE FEATURE POINTS */
    /* ---------------------------------------------------- */
	
    Mat temp80, temp160, temp170, temp90, temp180, temp190;
    cropBGRimgUp.copyTo(temp80);    cropDPTimgUp.copyTo(temp160);    minMaxIdx(temp160, &minDepth, &maxDepth);
    temp160.convertTo(temp170, CV_8UC3, 255/(maxDepth-minDepth), -minDepth);    applyColorMap(temp170, temp170, COLORMAP_RAINBOW);
    BGRimgUp.copyTo(temp90);    DPTimgUp.copyTo(temp180);    minMaxIdx(temp180, &minDepth, &maxDepth);
    temp180.convertTo(temp190, CV_8UC3, 255/(maxDepth-minDepth), -minDepth);    applyColorMap(temp190, temp190, COLORMAP_RAINBOW);

    for( int i = 0; i < deriZeroDPTimgUp.size(); i++ )
    {
      for( int j = 0; j < deriZeroDPTimgUp[i].size(); j++ )
      { // Displaying in the cropped image and the full image.
        Point2f pc( deriZeroDPTimgUp[i][j][0], deriZeroDPTimgUp[i][j][1] );        drawCenter(temp170, pc, Scalar(0,0,0));
        Point2f pf( deriZeroDPTfullImgUp[i][j][0], deriZeroDPTfullImgUp[i][j][1] );        drawCenter(temp190, pf, Scalar(0,0,0));
      }// for.
      Point2f pc( deriZeroDPTimgUp[i][0][0], (deriZeroDPTimgUp[i][1][2] + deriZeroDPTimgUp[i][2][2])/2 );
      drawCenter(temp170, pc, Scalar(0,0,0)); // Displaying average depth point midway between 2nd (1th) and 3rd (2th) point in cropped image.
      Point2f pf( deriZeroDPTfullImgUp[i][0][0], (deriZeroDPTfullImgUp[i][1][2] + deriZeroDPTfullImgUp[i][2][2])/2 );
      drawCenter(temp190, pf, Scalar(0,0,0)); // Displaying average depth point midway between 2nd (1th) and 3rd (2th) point in full image.
    }// for.

    /* ----------------------------------------------------------------------------------------------------------------------- */
    /* SETTING UP THE PARAMETERS AND DECIDING IF THIS STAIR IS MOVING UP AND MARKING THE FEATURE POINTS ON THE UP STAIRS IMAGE */
    /* ----------------------------------------------------------------------------------------------------------------------- */
    
	vector < vector<Vec4f> > scanPtsFINALup, scanPtsFINALfullImgUp;
    // These vectors hold actual x, y, average depth and average depth between first minima and second maxima of depth values of the scans
    // of the final selected points that qualifies the parameterized model.

    // CHECKING IF THE RECORDED DATA FALLS WITHIN THE PREDEFINED THRESHOLD OF THE PARAMETERIZED MODEL OF THE UP STAIRS.
    for( int i = 0; i < deriZeroDPTimgUp.size(); i++ )
    {
      // Calculating the estimated depths of the points.
      double estDepP1Up = PH00 + PH10 * deriZeroDPTimgUp[i][0][1] + PH20 * deriZeroDPTimgUp[i][0][2];
      double estDepP2Up = PH01 + PH11 * deriZeroDPTimgUp[i][0][1] + PH21 * deriZeroDPTimgUp[i][0][2];
      double estDisP2Up = PH02 + PH12 * deriZeroDPTimgUp[i][0][0] + PH22 * deriZeroDPTimgUp[i][0][1] + PH32 * deriZeroDPTimgUp[i][0][2];
      double estDepAvUp = PH03 + PH13 * deriZeroDPTimgUp[i][1][0] + PH23 * deriZeroDPTimgUp[i][1][1] + PH33 * deriZeroDPTimgUp[i][1][2];
      double slopBetwMaxiUp = (deriZeroDPTimgUp[i][0][2] - deriZeroDPTimgUp[i][2][2]) / (deriZeroDPTimgUp[i][0][1] - deriZeroDPTimgUp[i][2][1]);
      double slopBetwMiniUp = (deriZeroDPTimgUp[i][1][2] - deriZeroDPTimgUp[i][3][2]) / (deriZeroDPTimgUp[i][1][1] - deriZeroDPTimgUp[i][3][1]);

      // Checking if the actual depth is within the accepted threshold offsets from the estimated depth, if so then raise a flag.
      if( deriZeroDPTimgUp[i][1][2] < estDepP1Up + estDepP1HgOffSetUp && deriZeroDPTimgUp[i][1][2] > estDepP1Up + estDepP1LwOffSetUp )        flag0Up = 1;
      if( deriZeroDPTimgUp[i][2][2] < estDepP2Up + estDepP2HgOffSetUp && deriZeroDPTimgUp[i][2][2] > estDepP2Up + estDepP2LwOffSetUp )        flag1Up = 1;
      if( deriZeroDPTimgUp[i][2][1] < estDisP2Up + estDisP2HgOffSetUp && deriZeroDPTimgUp[i][2][1] > estDisP2Up + estDisP2LwOffSetUp )        flag2Up = 1;
      if( deriZeroDPTimgUp[i][1][3] < estDepAvUp + estDepAvHgOffSetUp && deriZeroDPTimgUp[i][1][3] > estDepAvUp + estDepAvLwOffSetUp )        flag3Up = 1;
      if( slopBetwMaxiUp <= 0 )        flag4Up = 1;
      if( slopBetwMiniUp <= 0 )        flag5Up = 1;

      // IF ALL THE CRITERIA ARE SATIFIED THEN DISPLAYING THE FEATURE POINTS IN THE IMAGE
      if( flag0Up > 0 && flag1Up > 0 && flag2Up > 0 && flag3Up > 0 && flag4Up > 0 && flag5Up > 0 ) // This means all the conditions are satisfied.
      { // The set of scanned points that staisfies the parameterized model are stored in a separate vector.
        scanPtsFINALup.push_back( deriZeroDPTimgUp[i] ); // This is relative to cropped image.
        scanPtsFINALfullImgUp.push_back( deriZeroDPTfullImgUp[i] ); // This is relative to full image.
      }// if.
      else
        cout << " " << flag0Up << flag1Up << flag2Up << flag3Up << flag4Up << flag5Up << " " << endl;
        ;
    }// for.

    // CALCULATING THE DISTANCE FORM THE UP STAIRS AND DISPLAYING THEM.
    // Parameters to calculate average distance of the person from stairs. Distances are in mm and angles are in degrees.
    double distanceUp = 0.0, qualifyPercentUp = 0.75;
    int upSampleCount = 0; // Flag to indicate that stairs moving up has been detected and also how many such scan samples are detected.

    if( scanPtsFINALup.size() >= qualifyPercentUp * scanPtsDPTimgUp.size() )
    { // Only if the 'qualifyPercentUp' percent of the scanned points satisfy the parameterized model, then display that stairs are detected.
      for( int i = 0; i<scanPtsFINALup.size(); i++ )
      {
        for( int j = 0; j < scanPtsFINALup[i].size(); j++ )
        { // Displaying in the cropped image and the full image.
          Point2f pc( scanPtsFINALup[i][j][0], scanPtsFINALup[i][j][1] );        drawCenter(temp80, pc, Scalar(255,255,0));
          Point2f pf( scanPtsFINALfullImgUp[i][j][0], scanPtsFINALfullImgUp[i][j][1] );        drawCenter(temp90, pf, Scalar(255,255,0));
        }// for.
        Point2f pc( scanPtsFINALup[i][0][0], (scanPtsFINALup[i][1][1] + scanPtsFINALup[i][2][1])/2 );
        drawCenter(temp80, pc, Scalar(255,255,0)); // Drawing average depth point midway between 2nd (1th) and 3rd (2th) point in cropped image.
        Point2f pf( scanPtsFINALfullImgUp[i][0][0], (scanPtsFINALfullImgUp[i][1][1] + scanPtsFINALfullImgUp[i][2][1])/2 );
        drawCenter(temp90, pf, Scalar(255,255,0)); // Drawing average depth point midway between 2nd (1th) and 3rd (2th) point in full image.

        // Calculating view angles from field of view (FOV).
        double verViewAngle = ( scanPtsFINALfullImgUp[i][0][1] - ACTUAL_HEIGHT/2 ) * DPT_VER_FOV / ACTUAL_HEIGHT ; // Vertical view angle.
        double horViewAngle = ( scanPtsFINALfullImgUp[i][0][0] - ACTUAL_WIDTH/2 ) * DPT_HOR_FOV / ACTUAL_WIDTH ; // Horizontal view angle.
        // The very first edge of the stairs is used to calculate the distance. When we are climbimg up the stairs, at every step the 1st 
        // edge of the next step is used to measure the distance.
        distanceUp += scanPtsFINALfullImgUp[i][0][2] * cos( (verViewAngle + mountAngleUp)*CV_PI/180 ) * cos( horViewAngle*CV_PI/180 );

        upSampleCount++; // Counting how many samples are being considered as detected stairs.
      }// for.

      // DISPLAYING THE DISTANCE OF UP STAIRS.
      distanceUp /= upSampleCount; // Calculating the average distance.
      if( (distanceUp > 100) && (distanceUp < 1000) )
      { // If the distance is between 100mm and 1000mm, then calculate in cm.
        sprintf(outputMsg, "Stairs Detected at %0.2f cm (UP)", 0.1*distanceUp);
        putText(temp90, outputMsg, Point( 20, ACTUAL_HEIGHT - 20 ), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(0,0,255), 4); // Writing a text.
      }// if.
      if( distanceUp > 1000 )
      { // If the distance is greater than 1000mm, then calculate in m.
        sprintf(outputMsg, "Stairs Detected at %0.2f m (UP)", 0.001*distanceUp);
        putText(temp90, outputMsg, Point( 20, ACTUAL_HEIGHT - 20 ), FONT_HERSHEY_SIMPLEX, 0.9, Scalar(0,0,255), 4); // Writing a text.
      }// if.
    }// if.

    /* --------------------------- */
    /* DISPLAYING THE FINAL IMAGES */
    /* --------------------------- */

    namedWindow("Image 1", 0); resizeWindow("Image 1", WIDTH, HEIGHT); moveWindow("Image 1", 500, 100); imshow("Image 1", temp60);
    namedWindow("Image 2", 0); resizeWindow("Image 2", WIDTH, HEIGHT); moveWindow("Image 2", 840, 100); imshow("Image 2", temp130);
    namedWindow("Image 5", 0); resizeWindow("Image 5", WIDTH, HEIGHT); moveWindow("Image 5", 500, 370); imshow("Image 5", temp90);
    namedWindow("Image 6", 0); resizeWindow("Image 6", WIDTH, HEIGHT); moveWindow("Image 6", 840, 370); imshow("Image 6", temp190);

    /* --------------- */
    /* KEYBOARD INPUTS */
    /* --------------- */

    key = waitKey(33);

    switch (key)
    {
      case 'n':        IDX++;        cout << " " << IDX << endl;        break;
      case 'p':        IDX--;        cout << " " << IDX << endl;        break;
      case 'm':        MODEL = 1;        cout << " In \"MODEL\" stairs detection mode... " << endl;        break;
      case 'r':        MODEL = 0;        cout << " In \"REAL\" stairs detection mode... " << endl;        break;
      case 's':
        saveIdx++;        char s[100];
        sprintf(s, "bgr_dn(%d).png", saveIdx);        imwrite(s, temp60);
        sprintf(s, "dpt_dn(%d).png", saveIdx);        imwrite(s, temp130);
        sprintf(s, "bgr_up(%d).png", saveIdx);        imwrite(s, temp90);
        sprintf(s, "dpt_up(%d).png", saveIdx);        imwrite(s, temp190);
        break;
      case 'q':        break;      default:         break;
    }// switch.
    if(key == 'q')
      break;

  }// while.

  destroyAllWindows();
  return 0;
}// main.




void drawCenter(Mat img, Point2f center, Scalar s)
{
  circle(img, center, 2, s, 2); // marking the point
  circle(img, center, 7, s, 1);
}// drawCenter.






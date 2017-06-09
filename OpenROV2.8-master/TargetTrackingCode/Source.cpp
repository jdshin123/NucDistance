#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include "RedObjectTracking.h"
#include "Target.h"
#include "Uuv.h"

#define PI 3.14159265

using namespace cv;
using namespace std;
using namespace cv;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 3;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 4; //1.5
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";
int xArrayBlack[MAX_NUM_OBJECTS];
int yArrayBlack[MAX_NUM_OBJECTS];
int xArrayRed[MAX_NUM_OBJECTS];
int yArrayRed[MAX_NUM_OBJECTS];
vector <Target> allVect;

void on_trackbar(int, void*)
{//This function gets called whenever a
 // trackbar position is changed

}
string intToString(int number) {


	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars() {
	//create window for trackbars


	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);


}
void drawObject(vector<Target> target, Mat &frame) {
	for (int i = 0; i < target.size(); i++) {
		std::ostringstream strs;
		strs << target.at(i).getAngle();
		std::string strAngle = strs.str();

		cv::circle(frame, cv::Point(target.at(i).getXPos(), target.at(i).getYPos()), 10, cv::Scalar(0, 0, 255));
		cv::putText(frame, intToString(target.at(i).getXPos()) + " , " + intToString(target.at(i).getYPos()), cv::Point(target.at(i).getXPos(), target.at(i).getYPos() + 20), 1, 1, Scalar(0, 255, 0));
		cv::putText(frame, target.at(i).getType(), cv::Point(target.at(i).getXPos(), target.at(i).getYPos() - 30), 1, 2, target.at(i).getColour());
		cv::putText(frame, strAngle, cv::Point(target.at(i).getXPos()+20, target.at(i).getYPos() ), 1, 2, Scalar(0,0,255));
		
	}
}
void morphOps(Mat &thresh) {

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);
}
void trackFilteredObject(Mat threshold, Mat HSV, Mat &cameraFeed) {
	vector <Target> targ;
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA) {

					Target targs;

					targs.setXPos(moment.m10 / area);
					targs.setYPos(moment.m01 / area);
					targ.push_back(targs);
					objectFound = true;

				}
				else objectFound = false;


			}
			//let user know you found an object
			if (objectFound == true) {
				//draw object location on screen
				drawObject(targ, cameraFeed);
			}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

void trackFilteredObject(Target targs,Mat threshold,Mat HSV,Mat &cameraFeed) {
	vector <Target> red; 
	int x, y;
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA) {
					//setting angle here
					Target reds;
					reds.setXPos(moment.m10 / area);
					reds.setYPos(y = moment.m01 / area);
					reds.setType(targs.getType());
					reds.setColour(targs.getColour());
					string strType = reds.getType();
					int xPos = reds.getXPos();
					int yPos = reds.getYPos();
					if (strType == "black")
					{
						//must do calc to fix value
						//reds.setAngle(130);
					}
					red.push_back(reds);
					allVect.push_back(reds);
					objectFound = true;
				}
				else objectFound = false;

			}
			//let user know you found an object
			if (objectFound == true) {
				//draw object location on screen
				//int slope = red.getXPos();
				int mX = red.at(1).getXPos() - red.at(0).getXPos();
				int mY = red.at(1).getYPos() - red.at(0).getYPos();

				//specifying specific conditions for the slope
				//when the difference of y is 0 and and the difference of x is positive
				if (mY = 0 && mX>0)
				{
					red.at(1).setAngle(180);
					red.at(0).setAngle(180);
				}
				if (mY = 0 && mX < 0)
				{
					red.at(1).setAngle(0);
					red.at(0).setAngle(0);
				}
				if (mX = 0 && mY>0)
				{
					red.at(1).setAngle(90);
					red.at(0).setAngle(90);
				}
				if (mX = 0 && mY < 0)
				{
					red.at(1).setAngle(270);
					red.at(0).setAngle(270);
				}
				if (mX != 0)
				{
					int slope = mY / mX;
					double angle = atan(slope) * 180 / PI;
					red.at(1).setAngle(angle);
					red.at(0).setAngle(angle);
				}

				drawObject(red, cameraFeed);
			}

		}
		//else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}


int main(int argc, char* argv[])
{
	//if we would like to calibrate our filter values, set to true.
	bool calibrationMode = false;

	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	Mat threshold;
	Mat HSV;

	if (calibrationMode) {
		//create slider bars for HSV filtering
		createTrackbars();
	}
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	
	capture.open(0);//"testing.MP4"
	
	//set height and width of capture frame
	//capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	//capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	float width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	float height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while (1) {
		//store image to matrix
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);

		if (calibrationMode == true) {
			//if in calibration mode, we track objects based on the HSV slider values.
			cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);

			//morphologoical opening(remove smalle objectsfrom teh foreground)
			//erode(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			//dilate(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

			//morphological closing fill in small holes of the foreground)
			//dilate(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			//erode(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

			morphOps(threshold);
			imshow(windowName2, threshold);
			trackFilteredObject(threshold, HSV, cameraFeed);
		}
		else 
		{
			//we are assuming Up is north, left is west, Right is east, South is down.
	
			Target red("red"), black("black");
			//red.setHSVmax(Scalar(10,255,255));
			//red.setHSVmin(Scalar(0, 100, 100));
			cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, red.getHSVmin(), red.getHSVmax(), threshold);
			
		
			//cout << ang << endl;
			

			morphOps(threshold);
			trackFilteredObject(red, threshold, HSV, cameraFeed);

			//black object, in this case our uuv
			//black.setHSVmin(Scalar(0, 0, 0));
			//black.setHSVmax(Scalar(180, 255, 30));
			cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
			inRange(HSV, black.getHSVmin(), black.getHSVmax(), threshold);
			morphOps(threshold);
			trackFilteredObject(black, threshold, HSV, cameraFeed);
			

			//morphologoical opening(remove smalle objectsfrom teh foreground)
			erode(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			dilate(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

			//morphological closing fill in small holes of the foreground)
			dilate(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
			erode(threshold, threshold, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

			black.setAngle(90);
			red.setAngle(120);


			
			

		}

		//show frames 
		imshow(windowName2,threshold);

		imshow(windowName, cameraFeed);
		imshow(windowName1,HSV);


		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		waitKey(30);
	}

	return 0;
}
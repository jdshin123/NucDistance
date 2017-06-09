#include "Target.h"



Target::Target()
{
	setType("null");
	setColour(Scalar(0, 0, 0));
	setAngle(0);

}

Target::Target(string name)
{
	if (name == "red") {
		setType("red");
		setHSVmax(Scalar(179, 255, 255));
		setHSVmin(Scalar(160, 100, 100));

		//setHSVmin(Scalar(0,152,92));
		//setHSVmax(Scalar(7,188,201));
		//cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
		//cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);

		//BGR value for RED:
		setColour(Scalar(0, 0, 255));
	}
	if (name == "yellow")
	{
		setType("yellow");
		setHSVmin(Scalar(0, 0, 0));
		setHSVmax(Scalar(255, 255, 255));

		//BGR value for Yellow:
		setColour(Scalar(0, 255, 255));
	}
	if (name == "black")
	{
		setHSVmin(Scalar(0, 0, 0,0));
		setHSVmax(Scalar(180, 255, 40,0));
		//testing for black color
		
		setType("black");
		//BGR value for black:
		setColour(Scalar(0, 0, 0)); 
	}
}
//setters and getters
void Target::setAngle(double a)
{
	Target::angle = a;
}
double Target::getAngle()
{
	return Target::angle;
}

 int Target::getXPos() {
	return Target::xPosition;
}
 void Target::setXPos(int x)
 {
	 Target::xPosition = x;
 }
 int Target::getYPos()
 {
	 return Target::yPosition;
 }
 void Target::setYPos(int y)
 {
	 Target::yPosition = y;
 }
 Scalar Target::getHSVmin()
 {
	 return Target::HSVmin;
 }
 Scalar Target::getHSVmax()
 {
	 return Target::HSVmax;
 }
 void  Target::setHSVmax(Scalar max)
 {
	 Target::HSVmax = max;
 }
 void Target::setHSVmin(Scalar min)
 {
	 Target::HSVmin = min;
 }

 
Target::~Target(void)
{
}


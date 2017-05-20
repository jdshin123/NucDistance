#include "Target.h"



Target::Target()
{
	setType("null");
	setColour(Scalar(0, 0, 0));
}

Target::Target(string name)
{
	if (name == "red") {
		setHSVmin(Scalar(0, 0, 0));
		setHSVmax(Scalar(255, 255, 255));
		//BGR value for RED:
		setColour(Scalar(255, 0, 0));
	}
	if (name == "yellow")
	{
		setHSVmin(Scalar(0, 0, 0));
		setHSVmax(Scalar(255, 255, 255));

		//BGR value for Yellow:
		setColour(Scalar(0, 255, 255));
	}
	if (name == "black")
	{
		setHSVmin(Scalar(0, 0, 0));
		setHSVmax(Scalar(255, 255, 255));

		//BGR value for black:
		setColour(Scalar(255, 255, 255));
	}
}
//setters and getters
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


#pragma once
#include <string>
#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace std;
using namespace cv;
class Target
{
public:
	Target();
	Target(string name);
	int getXPos();
	void setXPos(int x);

	int getYPos();
	void setYPos(int y);

	Scalar getHSVmin();
	Scalar getHSVmax();

	void setHSVmin(Scalar min);
	void setHSVmax(Scalar max);

	void setAngle(double a);
	double getAngle();
	string getType() { return type; }
	void setType(string t) { type = t; }

	Scalar getColour() {
		return Colour;
	}
	void setColour(Scalar c) {

		Colour = c;
	}
	


	~Target(void);
private:
	int xPosition;
	int yPosition;
	double angle;

	Scalar HSVmin, HSVmax;
	Scalar Colour;
	string type;


};


#ifndef __XYPosition_h__
#define __XYPosition_h__

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "Position.h"

class XYPosition : public Position {
public:
	XYPosition(double xi, double yi) :
		x(xi),
		y(yi) { }
	XYPosition() { x = 0; y = 0; }

	double X() { return x; }
	double Y() { return y; }

	std::string ToHRString() {
		char tmp[180];
		snprintf(tmp, 180, "X: %+11.6lf, Y: %+11.6lf", x, y);
		return std::string(tmp);
	}

	double Distance(const Position &op2) {
		const XYPosition &op = *((XYPosition *)&op2);
		return sqrt(pow(op.x - x, 2) + pow(op.y - y, 2));
	}

	Position &Project(const Heading h, const double distance) {
		x += distance * cos(h.GetHeadingR());
		y += distance * sin(h.GetHeadingR());
		return *this;
	}
	
	Heading AbsoluteBearing(const Position &op2) {
		const XYPosition &op = *((XYPosition *)&op2);
		return Heading(180.0/M_PI* atan2(op.y - y, op.x - x));
	}

private:
	double x;
	double y;
};

class XYPositionAndBearing : public PositionAndBearing<XYPosition> {
public:
	XYPositionAndBearing(double xi, double yi, double heading) :
		XYPosition(xi, yi),
		Heading(heading) { }
	
	XYPositionAndBearing() :
		XYPosition(0, 0),
		Heading(0) { }

	XYPosition Intersection(XYPositionAndBearing &other, bool closer = true) {
		// fix
		return XYPosition(0,0);
	}
private:
};

#endif

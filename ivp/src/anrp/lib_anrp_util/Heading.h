#ifndef __Heading_h__
#define __Heading_h__

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>

class Heading {
friend class Position;
public:
	Heading(double degs) {
		this->heading = M_PI/180.0 * degs;
		Correct();
	}
	Heading() { this->heading = 0; }
	~Heading() {}

	void AddHeading(Heading oh) { heading += oh.heading; Correct(); }
	void SubHeading(Heading oh) { heading -= oh.heading; Correct(); }

	Heading operator+(Heading oh) { return Heading(GetHeading() + oh.GetHeading()); }
	Heading operator-(Heading oh) { return Heading(GetHeading() - oh.GetHeading()); }

	void SetHeading(double h) {
		heading = h * M_PI/180.0;
		Correct();
	}

	double GetHeading() const { 
		double tmp = heading;
		if(tmp < 0) tmp += 2.0*M_PI;
		return tmp * 180.0/M_PI;
	}

	double GetHeadingR() const {
		return heading;
	}

private:
	double heading;
	void Correct() {
		while(heading > M_PI)  heading -= 2.0*M_PI;
		while(heading < -M_PI) heading += 2.0*M_PI;
	}
};

#endif

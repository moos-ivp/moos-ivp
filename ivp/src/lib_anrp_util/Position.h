#ifndef __Position_h__
#define __Position_h__

#include "Heading.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>

class Position {
public:
	virtual ~Position() { }

	virtual std::string ToHRString() = 0;

	virtual double Distance(const Position &op) = 0;
	virtual Position &Project(const Heading h, const double distance) = 0;
	virtual Heading AbsoluteBearing(const Position &op) = 0;
};

template <class Pos>
class PositionAndBearing : virtual public Pos, virtual public Heading {
public:
	Heading RelativeBearing(Pos op) {
		return AbsoluteBearing(op) - (*this);
	}

	virtual PositionAndBearing<Pos> &RelativeProjectPos(Heading h, double distance) {
		Pos::Project(h+Heading(*this), distance);
		return *this;
	}

	/*virtual PositionAndBearing<Pos> RelativeProject(Heading h, double distance) {
		PositionAndBearing<Pos> p = Pos::Project(h+Heading(*this), distance);
		p.SetHeading(h+Heading(*this));
		return *(static_cast<Pos *>(&p));
	}*/
	
	virtual Pos Intersection(PositionAndBearing<Pos> &, bool) {
		fprintf(stderr, "error, called template base class function\n");
		return Pos();
	}
};

#include "XYPosition.h"
#include "LLPosition.h"

#endif

#ifndef __LLPosition_h__
#define __LLPosition_h__

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>

static inline std::string dms(double a) {
	int d = int(floor(fabs(a)));
	
	double dm = (fabs(a) - floor(fabs(a)))*60.0;
	int min = (int)floor(dm);
	
	double dsec = (dm - double(min)) * 60.0;
	
	char tmp[180];
	snprintf(tmp, 180, "%c%3i deg %2i' %6.3lf\"", a<0.0? '-':'+', d, min, dsec);
	
	return std::string(tmp);
}

class LLPosition : public Position {
friend class LLPositionAndBearing;
public:
	LLPosition(double lat, double lon) {
		this->er = 6371000;
		this->lat = lat * M_PI/180.0;
		this->lon = lon * M_PI/180.0;
		Correct();
	}

	LLPosition() {
		this->er = 6371000;
		lat = lon = 0;
	}

	~LLPosition() {}

	double Latitude() { return lat * 180.0/M_PI; }
	double Lat() { return lat * 180.0/M_PI; }

	double Longitude() { return lon * 180.0/M_PI; }
	double Lon() { return lon * 180.0/M_PI; }

	std::string ToHRString() {
		char tmp[180];
		snprintf(tmp, 180, "Latitude: %+11.6lf, Longitude: %+11.6lf", lat*180.0/M_PI, lon*180.0/M_PI);
		return std::string(tmp);
	}

	std::string ToLLDMSString() {
		return "Latitude: " + dms(Lat()) + ", Longitude: " + dms(Lon());
	}		
	
	std::string ToMGRS() { return "unimpl"; }

	double Distance(const Position &op2) { // haversine, error up to 0.55%, generally <0.3% for flattened ellipsoid (spherical)
		LLPosition &op = *((LLPosition *)&op2);
		double dlat = op.lat - lat;
		double dlon = op.lon - lon;
		double a = pow(sin(dlat/2.0), 2.0) + cos(lat) * cos(op.lat) *
			pow(sin(dlon/2.0), 2.0);
		double c = 2.0 * atan2(sqrt(a), sqrt(1-a));
		return er * c;
	}

	Position &Project(const Heading h, const double distance) { // great circle projection (spherical)
		double lat2 = asin(sin(lat)*cos(distance/er) + cos(lat)*sin(distance/er)*cos(h.GetHeadingR()));
		double lon2 = lon + atan2(sin(h.GetHeadingR())*sin(distance/er)*cos(lat), cos(distance/er) - sin(lat)*sin(lat2));
		lat = lat2;
		lon = lon2;
		Correct();
		return *this;
	}
	
	Heading AbsoluteBearing(const Position &op2) { // (spherical)
		LLPosition &op = *((LLPosition *)&op2);
		double dlong = op.lon - lon;
		return Heading(180.0/M_PI* atan2(sin(dlong)*cos(op.lat), cos(lat)*sin(op.lat) - sin(lat)*cos(op.lat)*cos(dlong)));
	}

protected:
	double er;
	double lat;
	double lon;

private:
	void Correct() {
		while(lat < 0.0) lat += 2.0*M_PI;

		if(lat <= M_PI/2.0) { // OK
		} else if(lat > M_PI/2.0 && lat <= M_PI) {
			lat = M_PI - lat;
			lon += M_PI;
		} else if(lat > M_PI && lat <= 3.0*M_PI/2.0) {
			lat = -(lat - M_PI);
			lon += M_PI;
		} else if(lat > 3.0*M_PI/2.0) {
		}

		while(lat > M_PI/2.0)  lat -= 2*M_PI;
		while(lat < -M_PI/2.0) lat += 2*M_PI;

		while(lon > M_PI)  lon -= 2*M_PI;
		while(lon < -M_PI) lon += 2*M_PI;
	}
};

class LLPositionAndBearing : virtual public PositionAndBearing<LLPosition> {
public:
	LLPositionAndBearing(double lat, double lon, double head) :
		LLPosition(lat, lon),
		Heading(head) { }
	
	LLPositionAndBearing() :
		LLPosition(0, 0),
		Heading(0) { }
	
	LLPosition Intersection(LLPositionAndBearing &other, bool closer = true) {
		double lat1 = lat, lon1 = lon;
		LLPositionAndBearing tmp = *this;
		tmp.RelativeProjectPos(Heading(0), 1000);
		double lat2 = tmp.lat, lon2 = tmp.lon;
		double lat3 = other.lat, lon3 = other.lon;
		tmp = other;
		tmp.RelativeProjectPos(Heading(0), 1000);
		double lat4 = tmp.lat, lon4 = tmp.lon;
		// http://williams.best.vwh.net/intersect.htm

		double e1_e2[] = {
			sin(lat1-lat2) * sin((lon1+lon2)/2) * cos((lon1-lon2)/2) -
			sin(lat1+lat2) * cos((lon1+lon2)/2) * sin((lon1-lon2)/2),
			sin(lat1-lat2) * cos((lon1+lon2)/2) * cos((lon1-lon2)/2) +
			sin(lat1+lat2) * sin((lon1+lon2)/2) * sin((lon1-lon2)/2),
			cos(lat1) * cos(lat2) * sin(lon1-lon2),
			};
		double e3_e4[] = {
			sin(lat3-lat4) * sin((lon3+lon4)/2) * cos((lon3-lon4)/2) -
			sin(lat3+lat4) * cos((lon3+lon4)/2) * sin((lon3-lon4)/2),
			sin(lat3-lat4) * cos((lon3+lon4)/2) * cos((lon3-lon4)/2) +
			sin(lat3+lat4) * sin((lon3+lon4)/2) * sin((lon3-lon4)/2),
			cos(lat3) * cos(lat4) * sin(lon3-lon4),
			};
		
		double e12_l = sqrt(pow(e1_e2[0], 2) + pow(e1_e2[1], 2) + pow(e1_e2[2], 2));
		double e34_l = sqrt(pow(e3_e4[0], 2) + pow(e3_e4[1], 2) + pow(e3_e4[2], 2));

		double ea[] = {
			e1_e2[0] / e12_l,
			e1_e2[1] / e12_l,
			e1_e2[2] / e12_l,
			};

		double eb[] = {
			e3_e4[0] / e34_l,
			e3_e4[1] / e34_l,
			e3_e4[2] / e34_l,
			};

		double ep[] = {
			ea[1] * eb[2] - eb[1] * ea[2],
			ea[2] * eb[0] - eb[2] * ea[0],
			ea[0] * eb[1] - ea[1] * eb[0],
			};

		LLPosition p1 = LLPosition(atan2(ep[2], sqrt(pow(ep[0], 2) + pow(ep[1], 2))) * 180.0/M_PI, 180.0/M_PI*atan2(-ep[1], ep[0]));
		LLPosition p2 = LLPosition(-p1.Latitude(), p1.Longitude()+180);

		bool p1l = Distance(p1) > Distance(p2) ? true : false;

		if(p1l && !closer || !p1l && closer) return LLPosition(p1);
		else return LLPosition(p2);
	}
private:
};

#endif

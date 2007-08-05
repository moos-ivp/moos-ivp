#include <stdio.h>
#include <string.h>

#include "Position.h"

#ifdef TESTING

int main(int argc, char *argv[])
{
	// DMS test
	double dms_tests[] = {
		0.53756222222222222222, // 0deg, 32min, 15.224 sec
		0.5,
		-10,
		-100,
		100,
		360,
		};
	
	for(int i=0; i<sizeof(dms_tests)/sizeof(double); i++) {
		fprintf(stderr, "dms(%lf) =\n        >%s<\n", dms_tests[i], dms(dms_tests[i]).c_str());
	}

	double headings[] = {
		0,
		30,
		60,
		90,
		120,
		150,
		180,
		210,
		240,
		270,
		300,
		330,
		};
	
	LLPositionAndBearing zero(0, 0, 30);
	
	for(int i=0; i<sizeof(headings)/sizeof(double); i++) {
		LLPositionAndBearing np = zero;
		np.RelativeProjectPos(Heading(headings[i]), 10000000);
		fprintf(stderr, "projection from origin 1000m to %lf dir:\n   %s\n",
			headings[i], np.ToLLDMSString().c_str());
		fprintf(stderr, "   that position is %lf m from zero\n",
			zero.Distance(np));
		fprintf(stderr, "   that position has a bearing of %lf\n",
			zero.AbsoluteBearing(np).GetHeading());
	}

	LLPositionAndBearing ship(10, 10, 30);
	LLPosition object(10.5, 10.5);

	fprintf(stderr, "relative bearing of ship to object: %lf\n", ship.RelativeBearing(object).GetHeading());
	fprintf(stderr, "distance is %lf\n", ship.Distance(object));

	LLPosition cambridge(42.364379, -71.107635); // not the MOOS origin!
	LLPosition monterey(36.597613, -121.89949); // same

	fprintf(stderr, "bearing from cambridge to monterey is %lf\n", cambridge.AbsoluteBearing(monterey).GetHeading());
	fprintf(stderr, "distance is %lf\n", cambridge.Distance(monterey));

	LLPositionAndBearing obj1(0, -1,  45);
	LLPositionAndBearing obj2(0,  1, -45);
	
	LLPosition closer = obj1.Intersection(obj2, true), farther = obj1.Intersection(obj2, false);

	fprintf(stderr, "Intersection point is %s\n", closer.ToLLDMSString().c_str());
	fprintf(stderr, "Farther point is      %s\n", farther.ToLLDMSString().c_str());
	fprintf(stderr, "Distance is %lf\n", obj1.Distance(closer));
	fprintf(stderr, "Farther distance is %lf\n", obj1.Distance(farther));

	LLPosition pavilion_1(42.358645, -71.087035);
	LLPosition pavilion_2(42.358300, -71.088057);

	fprintf(stderr, "Distance/bearing for pavilion is %lf/%lf\n",
		pavilion_1.Distance(pavilion_2), pavilion_1.AbsoluteBearing(pavilion_2).GetHeading());
	
	LLPosition pavilion(42.35849, -71.08743);

	LLPosition buoys[12];

	buoys[0] = pavilion;
	buoys[0].Project(155, 30).Project(155+90, 5);
	buoys[1] = buoys[0];
	buoys[1].Project(155+90, 15);

	for(int i=1; i<5; i++) {
		buoys[i*2+0] = buoys[(i-1)*2+0];
		buoys[i*2+0].Project(155, 15);
		buoys[i*2+1] = buoys[(i-1)*2+1];
		buoys[i*2+1].Project(155, 15);
	}

	buoys[10] = buoys[0];
	buoys[10].Project(155, 15+(15/2)).Project(155+90, 15/2);
	buoys[11] = buoys[0];
	buoys[11].Project(155, 45+(15/2)).Project(155+90, 15/2);

	for(int i=0; i<12; i++) {
		fprintf(stderr, "Buoy %2i: %s\n", i+1, buoys[i].ToLLDMSString().c_str());
	}

	for(int i=0; i<12; i++) {
		fprintf(stderr, "Buoy %2i: %s\n", i+1, buoys[i].ToHRString().c_str());
	}

	LLPosition wo(42.1511388888889, -70.9555416666667);
	
	fprintf(stderr, "Origin: %s\n", wo.ToHRString().c_str());
	wo.Project(Heading(90), 1000); 
	fprintf(stderr, "Changed: %s\n", wo.ToHRString().c_str());
}

#endif // TESTING

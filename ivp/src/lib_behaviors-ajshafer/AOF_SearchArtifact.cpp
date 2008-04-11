/*****************************************************************/
/*    NAME: Andrew Shafer, Mike Benjamin                         */
/*    ORGN: MIT                                                  */
/*    FILE: AOF_SearchArtifact.cpp                               */
/*    DATE: NOV 28, 2007                                         */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <math.h> 
#include "AOF_SearchArtifact.h"
#include "AngleUtils.h"
#include "MBUtils.h"
#include "XYPolygon.h"

using std::vector;
using std::string;

//----------------------------------------------------------
// Procedure: Constructor

AOF_SearchArtifact::AOF_SearchArtifact(IvPDomain g_domain, const XYArtifactGrid *g_grid) : AOF(g_domain)
{
	crs_ix = g_domain.getIndex("course");
	spd_ix = g_domain.getIndex("speed");

	search_grid  = g_grid;
	time_horizon = 60;       // 60 Seconds by default
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_SearchArtifact::setParam(const string& param, double param_val)
{
	if(param == "os_lat") {
		os_lat = param_val;
		os_lat_set = true;
		return true;
	}
	else if(param == "os_lon") {
		os_lon = param_val;
		os_lon_set = true;
		return true;
	}
	else if(param == "time_horizon") {
		time_horizon = param_val;
		return true;
	}
	else if(param == "sensor_a") {
		sensor_a = param_val;
		return true;
	}
	else if(param == "sensor_b") {
		sensor_b = param_val;
		return true;
	}
	else
		return false;
}


//----------------------------------------------------------------
// Procedure: setParam

bool AOF_SearchArtifact::setParam(const string& param, 
			      const string& param_val)
{
	return false;
}


//----------------------------------------------------------------
// Procedure: initialize

bool AOF_SearchArtifact::initialize()
{
	if( (crs_ix==-1) || (spd_ix==-1) )
		return(false);

	if( !os_lat_set ||  !os_lon_set )
		return(false);

	return(true);
}


//----------------------------------------------------------------
// Procedure: fillCache
// Fills the ray_cache to contain vectors of ints,
// where each vector holds the indices of the cells that are
// intercepted along that direction.
void AOF_SearchArtifact::fillCache()
{
	ray_cache.clear();

	int i;
	double heading;
	double top_spd    = m_domain.getVarHigh(spd_ix);
	double top_dist   = top_spd * time_horizon;
	int heading_count = m_domain.getVarPoints(crs_ix);
	
	vector<int> blank;
	
	ray_cache.assign(heading_count, blank); // Make heading_count copies of a vector of ints
	
	// ...calculate the intercepted cells going that direction
	for(i=0; i<heading_count; i++) {
		bool ok = m_domain.getVal(crs_ix, i, heading);  // Puts a heading into variable heading

		if(!ok) { // Something's wrong...
			ray_cache.clear();
			return;
		}

		// Create a polygon that represents the most cells we can see
		XYPolygon poly;
		poly.add_vertex(0,0, false);
		poly.add_vertex(sensor_a,0, false);
		poly.add_vertex(sensor_a,top_dist, false);
		poly.add_vertex(0,top_dist, true); // now check convexity
		
		poly.new_center(os_lon, os_lat);
		poly.rotate(heading);

		// Each vector in ray_cache is a heading
		// Each int in the heading vector is the index of a square in the
		// search grid that is intercepted.
		int gsize = search_grid->size();
		for(int j=0; j<gsize; j++) {
			XYSquare square = search_grid->getElement(j);
			if(!square.containsPoint(os_lon,os_lat)) { // Don't count the current location
				if(poly.contains(square.getCenterX(), square.getCenterY())) {
					ray_cache[i].push_back(j);
				}
			}
		}
	}
}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given <Course, Speed, Time-on-leg> tuple 
//               given by a 3D ptBox (b).
//            Determines naut mile Closest-Point-of-Approach (CPA)
//               and returns a value after passing it thru the 
//               metric() function.

double AOF_SearchArtifact::evalBox(const IvPBox *b) const
{
	double eval_crs, eval_spd;

	double eval_tol = time_horizon;

	m_domain.getVal(crs_ix, b->pt(crs_ix,0), eval_crs); // Get the current course...
	m_domain.getVal(spd_ix, b->pt(spd_ix,0), eval_spd); // ...and speed
	
	double eval_dist = eval_tol * eval_spd;

	double gamOS  = degToRadians(eval_crs); // Angle in radians.
	double cgamOS = cos(gamOS);             // Cosine of Angle (osCRS).
	double sgamOS = sin(gamOS);             // Sine   of Angle (osCRS).

	double eval_lat = (cgamOS * eval_dist) + os_lat;
	double eval_lon = (sgamOS * eval_dist) + os_lon;

	double x1 = os_lon;
	double y1 = os_lat;
	double x2 = eval_lon;
	double y2 = eval_lat;

	double ret_val = 0;

	// Calculate the course index, crs_index, which is the integer
	// counterpart in the domain. 
	// For example if the domain is 0:359:360 then
	//    359 --> 359
	//    15  -->  15
	// For example if the domain is 0:359:180 then
	//    359 -->  89
	//    15  -->   7
	// For example if the domain is 0:359:720 then
	//    359 -->  718
	//    15  -->   30

	double crs_delta = m_domain.getVarDelta(crs_ix);
	int    cache_i   = (int)(eval_crs / crs_delta);
	int    vsize     = ray_cache[cache_i].size(); 

	double top_spd   = m_domain.getVarHigh(spd_ix);
	double top_dist  = top_spd * time_horizon;

	// For each intersected cell, calculate the value of that pass
	for(int cache_j=0; cache_j < vsize; cache_j++) {
		int grid_ix = ray_cache[cache_i][cache_j];
		XYSquare square = search_grid->getElement(grid_ix);
		double dist_to_square = square.ptDistToSquareCtr(x1, y1);
		// angle between the current cell and the eval_dist point
		double theta = degToRadians(angle360(relAng(x1, y1, x2, y2) - relAng(x1, y1, square.getCenterX(), square.getCenterY())));
		double proj_dist = dist_to_square * cos(theta);
		
		if( (eval_spd > 0) && (eval_dist > proj_dist) ) { // square is within this speed
			double clearance = search_grid->getClearance(grid_ix);
			double evalclearance = 1.0 - ((1.0 - clearance) * (1.0 - sensor_b));
			double delta_util = evalclearance - clearance;
			
			// Scale down delta_util based on distance, proj_dist
			
			ret_val += delta_util;
		}
	}

	return(ret_val);
}


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
#include <cmath>
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

	double heading;
	double top_spd    = m_domain.getVarHigh(spd_ix);
	double top_dist   = top_spd * time_horizon;
	double radius     = sqrt(top_dist * top_dist + (sensor_a * sensor_a / 4));
	uint heading_count = m_domain.getVarPoints(crs_ix);
	
	vector<uint> blank;
	
	ray_cache.assign(heading_count, blank); // Make heading_count copies of a vector of ints
	
//	// Each first level vector is a discrete course direction
//	// Each sub vector holds the index to searchgrid cells that are contained
//	// in the time horizon based on top-speed
//	std::vector<std::vector<uint> > ray_cache;
//	std::vector<uint>   local_indices; // Squares that are possible
//	std::vector<double> local_delP;    // Delta clearance for each of those squares
//	std::vector<double> local_dist;    // Distance from current loc to center of cell
//	std::vector<double> local_angle;   // Relative heading of cell from current location
	
	
	// 1) Make a smaller subset of cells to search
	// Find cells that are within radius of the current location
	uint gsize = search_grid->size();
	for(uint j=0; j<gsize; j++) {
			XYSquare square = search_grid->getElement(j);
			if(!square.containsPoint(os_lon,os_lat)) { // Don't count the current location
				double dist_to_square = square.ptDistToSquareCtr(os_lon, os_lat);
				if(dist_to_square < radius) { // Possible cell
					double clearance = search_grid->getClearance(j);
					//double delta_P = (1.0 - ((1.0 - clearance) * (1.0 - sensor_b))) - clearance;
					double delta_P = sensor_b * (1 - clearance); // equiv to above
					double ang = relAng(os_lon, os_lat, square.getCenterX(), square.getCenterY()); // degrees
					
					local_indices.push_back(j);
					local_delP.push_back(delta_P);
					local_dist.push_back(dist_to_square);
					local_angle.push_back(ang);
				}
			}
	}
	// Now the four local vars have our nearby values pre-computed
	
	// ...calculate the intercepted cells going that direction
	for(uint i=0; i<heading_count; i++) {
		bool ok = m_domain.getVal(crs_ix, i, heading);  // Puts a heading into variable heading

		if(!ok) { // Something's wrong...
			ray_cache.clear();
			return;
		}

		for(uint j=0; j < local_indices.size(); j++) { // Calculate if this square is visible
			// Remember that j must be dereferenced through the local_indices to get the actual square
			double ang = angle180(local_angle.at(j) - heading); // in degrees
			if( (ang < -90) || (ang > 90) ) continue; // Don't need to add things out of bounds
			double cang = cos(degToRadians(ang));
			double sang = sin(degToRadians(ang));
			double dist = local_dist.at(j);
			
			if ( (dist*cang < top_dist) && (std::abs(dist*sang) < (sensor_a/2)) ){
				ray_cache[i].push_back(j); // ray_cache holds local indices that reference grid indices
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
	uint    cache_i   = (int)(eval_crs / crs_delta);
	uint    vsize     = ray_cache[cache_i].size(); 

//	double top_spd   = m_domain.getVarHigh(spd_ix);
//	double top_dist  = top_spd * time_horizon;

	// For each intersected cell, calculate the value of that pass
	// We know that all cells in this course are within |A/2|, so only check the
	// evaluated distance
	for(uint h=0; h < vsize; h++) {
		double ang = angle180(local_angle.at(h) - eval_crs); // heading in degrees
		double cang = cos(degToRadians(ang));
		double dist = local_dist.at(h);
		
		if (dist*cang < eval_dist){
			ret_val += local_delP.at(h);
		}
	}
	
//	for(int cache_j=0; cache_j < vsize; cache_j++) {
//		int grid_ix = ray_cache[cache_i][cache_j];
//		XYSquare square = search_grid->getElement(grid_ix);
//		double dist_to_square = square.ptDistToSquareCtr(x1, y1);
//		// angle between the current cell and the eval_dist point
//		double theta = degToRadians(angle360(relAng(x1, y1, x2, y2) - relAng(x1, y1, square.getCenterX(), square.getCenterY())));
//		double proj_dist = dist_to_square * cos(theta);
//		
//		if( (eval_spd > 0) && (eval_dist > proj_dist) ) { // square is within this speed
//			double clearance = search_grid->getClearance(grid_ix);
//			double evalclearance = 1.0 - ((1.0 - clearance) * (1.0 - sensor_b));
//			double delta_util = evalclearance - clearance;
//			
//			// Scale down delta_util based on distance, proj_dist
//			
//			ret_val += delta_util;
//		}
//	}

	return(ret_val);
}

